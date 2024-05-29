#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "RDR3.Patches"

bool bBenchmarkOnly{};

static void ReadConfig()
{
    inipp::Ini<wchar_t> ini;
    // Initialize config
    std::wstring config_path = L"" PROJECT_NAME ".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        std::wstring ini_defaults = L"[Settings]\n"
            wstr(bBenchmarkOnly)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bBenchmarkOnly = true;
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bBenchmarkOnly), bBenchmarkOnly);
    }
    LOG(wstr(bBenchmarkOnly) " = %d\n", bBenchmarkOnly);
}

static void BranchFunction32(void* src, void* dst, int len)
{
    DWORD curProtection{};
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);
    memset(src, 0x90, len);
    uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;
    *(BYTE*)src = 0xE9;
    *(uint32_t*)((uintptr_t)src + 1) = relativeAddress;
    DWORD temp{};
    VirtualProtect(src, len, curProtection, &temp);
}

uintptr_t g_nextScrOpcodeReturnAddr = 0;

static void __attribute__((naked)) Opcode6cAsm()
{
    __asm
    {
        SUB RBX, 0x10;
        INC RDI;
        MOVZX EDX, byte ptr[RDI];
        MOV RCX, qword ptr[R8];
        MOV RAX, qword ptr[RBX + 0x8];
        MOV qword ptr[RCX + RDX * 0x8], RAX;
        // some fixed constants within `benchmark.ysc` data section
        cmp dword ptr[rcx + 0x438], 0x3C42A781;
        jnz not_match;
        cmp dword ptr[rcx + 0x440], 0x08FA8CCA;
        jnz not_match;
        mov dword ptr[rcx + 0x1090], 5; // benchmark sequence override
    not_match:;
        jmp qword ptr[rip + g_nextScrOpcodeReturnAddr];
    }
}

static void ApplyPatches()
{
    // TODO: load directly into benchmark if this bool is enabled
    if (bBenchmarkOnly)
    {
        uintptr_t jumpPattern = FindAndPrintPatternW(L"cc cc cc cc cc cc cc cc cc cc cc cc cc cc", L"Int3 Array");
        uintptr_t OpcodePattern = FindAndPrintPatternW(L"48 83 eb 10 48 ff c7 0f b6 17", L"6c Opcode finder");
        g_nextScrOpcodeReturnAddr = FindAndPrintPatternW(L"4c 8b 75 67 4c 8b 7c 24 38 4c 8b c3 48 ff c7", L"nextScrOpcode", +9);
        if (OpcodePattern && jumpPattern && g_nextScrOpcodeReturnAddr)
        {
            BranchFunction32((void*)OpcodePattern, (void*)jumpPattern, 10);
            Memory::DetourFunction64((void*)jumpPattern, (void*)Opcode6cAsm, 14);
        }
    }
}

static void Main()
{
    baseModule = GetModuleHandle(NULL);
    ReadConfig();
    ApplyPatches();
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        Main();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return 1;
}
