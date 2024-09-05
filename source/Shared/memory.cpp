#include "memory.hpp"

// TODO: Find out where this came from
// https://github.com/Lyall/IshinFix/blob/eae0caeecdd17e2959e1a44802c6ea54e19fee03/src/helper.hpp#L6
// where i found it.
namespace Memory
{

    void PatchBytes(uintptr_t address, const void* pattern, unsigned int numBytes)
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

    bool DetourFunction32(void* src, void* dst, int len)
    {

        DWORD curProtection;
        VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

        memset(src, 0x90, len);

        uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;

        *(BYTE*)src = 0xE9;
        *(uint32_t*)((uintptr_t)src + 1) = relativeAddress;

        DWORD temp;
        VirtualProtect(src, len, curProtection, &temp);

        return true;
    }

    void* DetourFunction64(void* pSource, void* pDestination, DWORD dwLen)
    {
        constexpr DWORD MinLen = 14;

        if (dwLen < MinLen)
        {
            return nullptr;
        }

        BYTE stub[] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$+6]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
        };

        DWORD dwOld = 0;
        VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

        // orig
        memcpy(stub + 6, &pDestination, 8);
        memcpy(pSource, stub, sizeof(stub));

        for (DWORD i = MinLen; i < dwLen; i++)
        {
            *(BYTE*)((DWORD_PTR)pSource + i) = 0x90;
        }

        VirtualProtect(pSource, dwLen, dwOld, &dwOld);
        return pDestination;
    }

    bool CallFunction32(void* src, void* dst, int len)
    {
        if (!src || !dst || len < 5)
        {
            return false;
        }
        DWORD curProtection;
        VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

        memset(src, 0x90, len);

        uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;

        *(BYTE*)src = 0xE8;
        *(uint32_t*)((uintptr_t)src + 1) = relativeAddress;

        DWORD temp;
        VirtualProtect(src, len, curProtection, &temp);

        return true;
    }

    void* CallFunction64(void* pSource, void* pDestination, DWORD dwLen)
    {
        constexpr DWORD MinLen = 16;

        if (! pSource || !pDestination || dwLen < MinLen)
        {
            return nullptr;
        }

        BYTE stub[] = {
        0xFF, 0x15, 0x02, 0x00, 0x00, 0x00, // call qword ptr [$+8]
        0xEB, 0x08, // jmp $+8
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
        };

        DWORD dwOld = 0;
        VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

        // orig
        memcpy(stub + 8, &pDestination, 8);
        memcpy(pSource, stub, sizeof(stub));

        for (DWORD i = MinLen; i < dwLen; i++)
        {
            *(BYTE*)((DWORD_PTR)pSource + i) = 0x90;
        }

        VirtualProtect(pSource, dwLen, dwOld, &dwOld);
        return pDestination;
    }

    HMODULE GetThisDllHandle()
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

    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    // Unicode Version
    std::uint8_t* PatternScanW(void* module, const wchar_t* signature)
    {
        static auto pattern_to_byte = [](const wchar_t* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<wchar_t*>(pattern);
            auto end = const_cast<wchar_t*>(pattern) + wcslen(pattern);

            for (auto current = start; current < end; ++current) {
                if (*current == L'?') {
                    ++current;
                    if (*current == L'?')
                        ++current;
                    bytes.push_back(-1);
                }
                else {
                    bytes.push_back(wcstoul(current, &current, 16));
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

    uint8_t* char_Scan(void* module, const char* value, size_t value_len)
    {
        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
        auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((std::uint8_t*)module + dosHeader->e_lfanew);
        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);
        for (size_t i = 0; i < sizeOfImage - value_len; ++i) {
            const char* currentValue = reinterpret_cast<const char*>(&scanBytes[i]);
            if (strncmp(currentValue, value, value_len) == 0) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }

    uint8_t* u32_Scan(void* module, uint32_t value)
    {
        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
        auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((std::uint8_t*)module + dosHeader->e_lfanew);
        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        for (size_t i = 0; i < sizeOfImage - sizeof(value); ++i) {
            auto currentValue = *reinterpret_cast<uint32_t*>(&scanBytes[i]);
            if (currentValue == value) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }

    uint8_t* u64_Scan(void* module, uint64_t value)
    {
        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
        auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((std::uint8_t*)module + dosHeader->e_lfanew);
        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        for (size_t i = 0; i < sizeOfImage - sizeof(uint64_t); ++i) {
            auto currentValue = *reinterpret_cast<uint64_t*>(&scanBytes[i]);
            if (currentValue == value) {
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
        int len = _snprintf_s(buf, sizeof(buf), _TRUNCATE, "%hu.%hu.%hu.%hu",
            HIWORD(pvFileInfo->dwFileVersionMS),
            LOWORD(pvFileInfo->dwFileVersionMS),
            HIWORD(pvFileInfo->dwFileVersionLS),
            LOWORD(pvFileInfo->dwFileVersionLS)
        );

        return std::string(buf, len);
    }

    std::vector<int> string_to_ints(const std::string& s, char delimiter) {
        std::vector<int> tokens;
        std::string token;
        std::istringstream tokenStream(s);
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

        if (cbTranslate < sizeof(LANGCODEPAGE))
            return L"";

        wchar_t query[512] = { 0 };
        _snwprintf_s(query, _countof(query), _TRUNCATE, L"\\StringFileInfo\\%04x%04x\\ProductName",
            lpTranslate[0].wLang,
            lpTranslate[0].wCode);
        wchar_t const* p;
        UINT n_chars;
        VerQueryValue(buffer.data(), query, (LPVOID*)&p, &n_chars);
        return std::wstring(p, p + n_chars);
    }
}
