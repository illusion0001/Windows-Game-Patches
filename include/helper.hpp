#include "stdafx.h"
#include <stdio.h>

using namespace std;

// TODO: Find out where this came from
namespace Memory
{
    template<typename T>
    void Write(uintptr_t* writeAddress, T value)
    {
        DWORD oldProtect;
        VirtualProtect((LPVOID)(writeAddress), sizeof(T), PAGE_EXECUTE_WRITECOPY, &oldProtect);
        *(reinterpret_cast<T*>(writeAddress)) = value;
        VirtualProtect((LPVOID)(writeAddress), sizeof(T), oldProtect, &oldProtect);
    }

    void PatchBytes(uintptr_t address, const char* pattern, unsigned int numBytes)
    {
        DWORD oldProtect;
        VirtualProtect((LPVOID)address, numBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID)address, pattern, numBytes);
        VirtualProtect((LPVOID)address, numBytes, oldProtect, &oldProtect);
    }

    void ReadBytes(const uintptr_t address, void* const buffer, const SIZE_T size) 
    {
        memcpy(buffer, reinterpret_cast<const void*>(address), size); 
    }

    uintptr_t ReadMultiLevelPointer(uintptr_t base, const std::vector<uint32_t>& offsets)
    {
        MEMORY_BASIC_INFORMATION mbi;
        for (auto& offset : offsets)
        {
            if (!VirtualQuery(reinterpret_cast<LPCVOID>(base), &mbi, sizeof(MEMORY_BASIC_INFORMATION)) || mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))
                return 0;

            base = *reinterpret_cast<uintptr_t*>(base) + offset;
        }

        return base;
    }

    int GetHookLength(char* src, int minLen)
    {
        int lengthHook = 0;
        const int size = 15;
        char buffer[size];

        memcpy(buffer, src, size);

        // must be >= 14
        while (lengthHook <= minLen)
            lengthHook += ldisasm(&buffer[lengthHook], true);

        return lengthHook;
    }

    bool DetourFunction32(void* src, void* dst, int len)
    {
        if (len < 5) return false;

        DWORD curProtection;
        VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

        memset(src, 0x90, len);

        uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;

        *(BYTE*)src = 0xE9;
        *(uintptr_t*)((uintptr_t)src + 1) = relativeAddress;

        DWORD temp;
        VirtualProtect(src, len, curProtection, &temp);

        return true;
    }

    void* DetourFunction64(void* pSource, void* pDestination, int dwLen)
    {
        DWORD MinLen = 14;

        if (dwLen < MinLen) return NULL;

        BYTE stub[] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$+6]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
        };

        void* pTrampoline = VirtualAlloc(0, dwLen + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

        DWORD dwOld = 0;
        VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

        DWORD64 retto = (DWORD64)pSource + dwLen;

        // trampoline
        memcpy(stub + 6, &retto, 8);
        memcpy((void*)((DWORD_PTR)pTrampoline), pSource, dwLen);
        memcpy((void*)((DWORD_PTR)pTrampoline + dwLen), stub, sizeof(stub));

        // orig
        memcpy(stub + 6, &pDestination, 8);
        memcpy(pSource, stub, sizeof(stub));

        for (int i = MinLen; i < dwLen; i++)
        {
            *(BYTE*)((DWORD_PTR)pSource + i) = 0x90;
        }

        VirtualProtect(pSource, dwLen, dwOld, &dwOld);
        return (void*)((DWORD_PTR)pTrampoline);
    }

    static HMODULE GetThisDllHandle()
    {
        MEMORY_BASIC_INFORMATION info;
        size_t len = VirtualQueryEx(GetCurrentProcess(), (void*)GetThisDllHandle, &info, sizeof(info));
        assert(len == sizeof(info));
        return len ? (HMODULE)info.AllocationBase : NULL;
    }

    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    std::uint8_t* PatternScan(void* module, const char* signature)
    {
        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for (auto current = start; current < end; ++current) {
                if (*current == '?') {
                    ++current;
                    if (*current == '?')
                        ++current;
                    bytes.push_back(-1);
                }
                else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for (auto i = 0ul; i < sizeOfImage - s; ++i) {
            bool found = true;
            for (auto j = 0ul; j < s; ++j) {
                if (scanBytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }

    std::string GetVersionString()
    {
        auto hInst = GetModuleHandle(NULL);
        auto hResInfo = FindResourceW(hInst, MAKEINTRESOURCE(1), RT_VERSION);
        if (!hResInfo) return {};
        auto dwSize = SizeofResource(hInst, hResInfo);
        if (!dwSize) return {};
        auto hResData = LoadResource(hInst, hResInfo);
        char* pRes = static_cast<char*>(LockResource(hResData));
        if (!pRes) return {};

        std::vector<char> ResCopy(pRes, pRes + dwSize);

        VS_FIXEDFILEINFO* pvFileInfo;
        UINT uiFileInfoLen;

        if (!VerQueryValueA(ResCopy.data(), "\\", reinterpret_cast<void**>(&pvFileInfo), &uiFileInfoLen))
            return "(unknown)";

        char buf[25];
        int len = sprintf(buf, "%hu.%hu.%hu.%hu",
            HIWORD(pvFileInfo->dwFileVersionMS),
            LOWORD(pvFileInfo->dwFileVersionMS),
            HIWORD(pvFileInfo->dwFileVersionLS),
            LOWORD(pvFileInfo->dwFileVersionLS)
        );

        return std::string(buf, len);
    }

    vector<int> string_to_ints(const string& s, char delimiter) {
        vector<int> tokens;
        string token;
        istringstream tokenStream(s);
        while (getline(tokenStream, token, delimiter)) {
            tokens.push_back(stoi(token));
        }
        return tokens;
    }


    std::wstring GetVersionProductName()
    {
        struct LANGCODEPAGE {
            WORD wLang;
            WORD wCode;
        } *lpTranslate;

        UINT cbTranslate;

        HMODULE baseModule = GetModuleHandle(NULL);

        auto exePath = new WCHAR[_MAX_PATH];
        GetModuleFileName(baseModule, exePath, _MAX_PATH);
        auto size = GetFileVersionInfoSize(exePath, NULL);
        std::vector<char> buffer(size);
        GetFileVersionInfo(exePath, NULL, size, buffer.data());
        VerQueryValue(buffer.data(), L"VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate);

        if (cbTranslate < sizeof(LANGCODEPAGE)) {
            return L"";
        }

        wchar_t query[512];
        _snwprintf(query, sizeof(query), L"\\StringFileInfo\\%04x%04x\\ProductName", lpTranslate[0].wLang, lpTranslate[0].wCode);
        wchar_t const* p;
        UINT n_chars;
        VerQueryValue(buffer.data(), query, (LPVOID*)&p, &n_chars);
        return std::wstring(p, p + n_chars);
    }
}
