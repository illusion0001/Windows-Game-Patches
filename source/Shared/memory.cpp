#include "memory.hpp"
#include "HDE/Table64.h"
#include "HDE/HDE64.h"
#include "HDE/HDE64.c"

// TODO: Find out where this came from
// https://github.com/Lyall/IshinFix/blob/eae0caeecdd17e2959e1a44802c6ea54e19fee03/src/helper.hpp#L6
// where i found it.
namespace Memory
{

    void MakeNops(const uintptr_t address, const int numOfNops)
    {
        FillInt(address, 0x90, numOfNops);
    }
    void FillInt(const uintptr_t address, const int intNum, const int numOfNops)
    {
        if (!address || !numOfNops)
        {
            return;
        }
        DWORD curProtection{};
        VirtualProtect((LPVOID)address, numOfNops, PAGE_EXECUTE_READWRITE, &curProtection);
        memset((LPVOID)address, intNum, numOfNops);
        VirtualProtect((LPVOID)address, numOfNops, curProtection, &curProtection);
        printf_s("Write %d bytes of 0x%08x to 0x%llx\n", numOfNops, intNum, address);
    }
    void PatchBytes(uintptr_t address, uintptr_t pattern, unsigned int numBytes)
    {
        PatchBytes(address, (void*)pattern, numBytes);
    }
    void PatchBytes(void* address, const void* pattern, unsigned int numBytes)
    {
        PatchBytes((uintptr_t)address, pattern, numBytes);
    }
    void PatchBytes(uintptr_t address, const void* pattern, unsigned int numBytes)
    {
        if (!address || !pattern || !numBytes)
        {
            return;
        }
        DWORD oldProtect{}, temp{};
        VirtualProtect((LPVOID)address, numBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID)address, pattern, numBytes);
        VirtualProtect((LPVOID)address, numBytes, oldProtect, &temp);
        printf_s("Write %d bytes of 0x%llx\n", numBytes, address);
    }

    void nPatchBytes(void* address, const void* pattern, unsigned int numBytes, int toPtr)
    {
        memcpy(address, toPtr ? &pattern : pattern, numBytes);
    }
    void nReadBytes(const uintptr_t address, void* const buffer, const SIZE_T size)
    {
        memcpy(buffer, reinterpret_cast<const void*>(address), size);
    }
    void ReadBytes(const uintptr_t address, void* const buffer, const SIZE_T size)
    {
        if (!address || !buffer || !size)
        {
            return;
        }
        DWORD oldProtect{}, temp{};
        VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy(buffer, reinterpret_cast<const void*>(address), size);
        VirtualProtect((LPVOID)address, size, oldProtect, &temp);
        printf_s("Read %lld bytes of 0x%llx to 0x%p\n", size, address, buffer);
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

    constexpr int jmpcallbytes = 5;

    void DetourFunction32(void* src, void* dst, int len)
    {

        if (!src || !dst || len < jmpcallbytes)
        {
            return;
        }
        MakeNops((uintptr_t)src, len);
        DWORD curProtection;
        VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

        const uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - jmpcallbytes;

        *(BYTE*)src = 0xE9;
        *(uint32_t*)((uintptr_t)src + 1) = relativeAddress;

        DWORD temp;
        VirtualProtect(src, len, curProtection, &temp);

    }

    void DetourFunction32(uintptr_t src, uintptr_t dst, int len)
    {
        if (!src || !dst || len < jmpcallbytes)
        {
            return;
        }
        DetourFunction32((void*)src, (void*)dst, len);
    }

    const static BYTE JMPstub[] = {
    0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$+6]
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
    };

    const static BYTE CALLstub[] = {
    0xFF, 0x15, 0x02, 0x00, 0x00, 0x00, // call qword ptr [$+8]
    0xEB, 0x08, // jmp $+8
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
    };

    void DetourFunction64(const void* pSource, const void* pDestination, const DWORD dwLen, const BOOL callMode)
    {

        const BYTE* stub = callMode ? CALLstub : JMPstub;
        const size_t stubSize = callMode ? sizeof(CALLstub) : sizeof(JMPstub);
        const size_t stubOffset = stubSize - sizeof(uintptr_t);

        if (!pSource || !pDestination || dwLen < stubSize)
        {
            return;
        }
        const uintptr_t upSource = (uintptr_t)pSource;
        MakeNops(upSource, dwLen);
        PatchBytes(upSource, stub, stubSize);
        PatchBytes(upSource + stubOffset, &pDestination, sizeof(pDestination));
    }
    void DetourFunction64(uintptr_t pSource, uintptr_t pDestination, DWORD dwLen)
    {
        DetourFunction64((void*)pSource, (void*)pDestination, dwLen);
    }

    void CallFunction32(void* src, void* dst, int len)
    {
        if (!src || !dst || len < jmpcallbytes)
        {
            return;
        }
        DetourFunction32(src, dst, len);
        static const uint8_t call[] = { 0xe8 };
        PatchBytes((uintptr_t)src, call, sizeof(call));
    }

    void CallFunction64(void* pSource, void* pDestination, DWORD dwLen)
    {
        DetourFunction64(pSource, pDestination, dwLen, true);
    }

    // https://github.com/GoldHEN/GoldHEN_Plugins_SDK/blob/bcea3c7ef01dac6d7f9f49ebf9e90fe66d86f5f7/source/Detour.c#L26-L41
    static size_t GetInstructionSize(uint64_t Address, size_t MinSize)
    {
        size_t InstructionSize = 0;

        if (!Address)
        {
            return 0;
        }

        while (InstructionSize < MinSize)
        {
            // hde64_disasm always clears it
            hde64s hs{};
            uint32_t temp = hde64_disasm((void*)(Address + InstructionSize), &hs);

            if (hs.flags & F_ERROR)
            {
                return 0;
            }

            InstructionSize += temp;
        }

        return InstructionSize;
    }

    static size_t caveInstSize = 0;
    static void CaveBlockInit()
    {
        static BOOL once{};
        if (!once)
        {
            memset(cavePad, 0xcc, sizeof(cavePad));
            DWORD temp = caveInstSize = 0;
            VirtualProtect(cavePad, sizeof(cavePad), PAGE_EXECUTE_WRITECOPY, &temp);
            once = true;
            printf_s("cavePad setup at 0x%p! Size %lld\n", cavePad, sizeof(cavePad));
        }
    }
    static BOOL validnateBlockSize(const size_t newSize)
    {
        if (newSize > sizeof(cavePad))
        {
            printf_s("Block size %lld is smaller than requested size %lld!\n", sizeof(cavePad), newSize);
            return 0;
        }
        return 1;
    }
    const size_t addroffset = (sizeof(JMPstub) - sizeof(uintptr_t));
    // Allocation-less version of a Prologue hook
    // I use pre-allocated `returnPad`, copy instructions to it and write instructions to it
    uintptr_t CreatePrologueHook(const uintptr_t address, const int min_instruction_size)
    {
        CaveBlockInit();
        if (!address || min_instruction_size < 5)
        {
            return 0;
        }
        const size_t int_size = GetInstructionSize(address, min_instruction_size);
        if (!int_size || !validnateBlockSize(caveInstSize + int_size + sizeof(JMPstub)))
        {
            return 0;
        }
        const uintptr_t ucavePad = (uintptr_t)&cavePad;
        const uintptr_t ucavePadNew = ucavePad + caveInstSize;
        const uintptr_t retaddr = address + int_size;
        // prevents memcpy inlining
        PatchBytes(ucavePadNew, address, int_size);
        PatchBytes(ucavePadNew + int_size, &JMPstub, addroffset);
        PatchBytes(ucavePadNew + int_size + addroffset, &retaddr, sizeof(retaddr));
        const uintptr_t caveAddr = ucavePad + caveInstSize;
        caveInstSize += int_size + sizeof(JMPstub);
        printf_s("New caveInstSize: %lld\n", caveInstSize);
        return caveAddr;
    }
    uintptr_t CreateCodeCaveBlockX64(const void* data, const uintptr_t retaddr, const size_t data_size)
    {
        CaveBlockInit();
        if (!validnateBlockSize(caveInstSize + data_size + sizeof(JMPstub)))
        {
            return 0;
        }
        if (retaddr)
        {
            const uintptr_t ucavePad = (uintptr_t)&cavePad + caveInstSize;
            const uintptr_t pRet = retaddr;
            Memory::PatchBytes(ucavePad, data, data_size);
            Memory::PatchBytes(ucavePad + data_size, JMPstub, sizeof(JMPstub));
            Memory::PatchBytes(ucavePad + data_size + addroffset, &pRet, sizeof(pRet));
            caveInstSize += data_size + sizeof(JMPstub);
            printf_s("Uploaded %lld bytes to cave 0x%llx\n", data_size, ucavePad);
            printf_s("New caveInstSize: %lld\n", caveInstSize);
            return ucavePad;
        }
        return 0;
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

    uint8_t* char_Scan(void* module, const char* value)
    {
        return char_Scan(module, value, strlen(value));
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

    uint8_t* wchar_Scan(void* module, const wchar_t* value)
    {
        return wchar_Scan(module, value, wcslen(value));
    }

    uint8_t* wchar_Scan(void* module, const wchar_t* value, size_t value_len)
    {
        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
        auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((std::uint8_t*)module + dosHeader->e_lfanew);
        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);
        for (size_t i = 0; i < sizeOfImage - value_len; ++i)
        {
            const wchar_t* currentValue = reinterpret_cast<const wchar_t*>(&scanBytes[i]);
            if (wcscmp(currentValue, value) == 0)
            {
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

#if !defined(WINXP)
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
#endif
}
