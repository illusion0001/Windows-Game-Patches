#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"
#include "IConsole.h"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "Crysis3Remastered.Patches"

enum eBenchmarkTypes
{
    eCPUBenchmark = 1,
    eGPUBenchmark,
};

// INI Variables
bool bBenchmarkingOnly{};
bool bVerboseConsoleOutput{};
int iBenchmarkType{};

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Initialize config
    std::wstring config_path = L"" PROJECT_NAME ".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        std::wstring ini_defaults = L"[Settings]\n"
            wstr(bBenchmarkingOnly)" false\n"
            wstr(bVerboseConsoleOutput)" = false\n"
            "; Benchmark types:\n"
            "; 1 = CPU Benchmark\n"
            "; 2 = GPU Benchmark\n"
            "; Savefiles from `zip:\\x64\\Crysis3Remastered.Patches\\SaveGames\\` must be installed to `%userprofile%\\Saved Games\\Crysis3Remastered\\SaveGames\\`\n"
            wstr(iBenchmarkType)" = 1\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bBenchmarkingOnly = false;
        bVerboseConsoleOutput = false;
        iBenchmarkType = 1;
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bBenchmarkingOnly), bBenchmarkingOnly);
        inipp::get_value(ini.sections[L"Settings"], wstr(bVerboseConsoleOutput), bVerboseConsoleOutput);
        inipp::get_value(ini.sections[L"Settings"], wstr(iBenchmarkType), iBenchmarkType);
    }
}

#define TYPEDEF_FUNCTION_PTR(ret_type, func_name, ...) \
    typedef ret_type (*func_name##_ptr)(__VA_ARGS__); \
    extern func_name##_ptr func_name;

#define INIT_FUNCTION_PTR(func_name) \
    func_name##_ptr func_name = nullptr

TYPEDEF_FUNCTION_PTR(void, CryLogAlways_, const char* fmt, ...);
INIT_FUNCTION_PTR(CryLogAlways_);

struct ISystem
{
    IConsole* pConsole;
};

ISystem** g_System;
bool saveloaded{};

namespace CRichPresence
{
    static bool SetRichPresence_Hook(const char* msg)
    {
        CryLogAlways_(msg);
        if (!saveloaded)
        {
            CryLogAlways_("CRichPresence::SetRichPresence_Hook: Executing benchmark\n");
            g_System[0]->pConsole->ExecuteString(g_System[0], iBenchmarkType == eCPUBenchmark ? "load default_CPU.CSF" : "load default_GPU.CSF", true, false);
            g_System[0]->pConsole->ExecuteString(g_System[0], "con_restricted 0", true, false);
            // why is this considered restricted??
            g_System[0]->pConsole->ExecuteString(g_System[0], "g_skipAfterLoadingScreen 1", true, false);
            saveloaded = true;
        }
        return 1;
    }
};


static bool CallFunction32(void* src, void* dst, int len)
{
    if (len < 5)
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

static void DoPatches()
{
    // kills two bird with one stone
    // patches to load our custom save
    // and redirect CryLog to CryLogAlways
    // https://github.com/ValtoGameEngines/CryEngine/blob/d9d2c9f000836f0676e65a90bed40dcc3b1451eb/Code/GameSDK/GameDll/RichPresence.cpp#L75-L79
    // if (!g_pGame->HasExclusiveControllerIndex())
    // {
    //        CryLog("[Rich Presence] not setting rich presence, no player set");
    //        return true;
    // }
    uintptr_t AfterLogosHook = FindAndPrintPatternW(L"48 8d 0d ? ? ? ? e8 ? ? ? ? b0 01 48 81 c4 ? ? ? ? 41 5e 5f 5d c3", L"RichPersistence Pattern");
    uintptr_t g_SystemAddr = FindAndPrintPatternW(L"48 8b 0d ? ? ? ? 48 8d 15 ? ? ? ? 45 33 c9 45 33 c0 48 8b 01 ff 90 10 01 00 00", L"ISystem Global Ptr");
    uintptr_t CryLogAlways_Addr = FindAndPrintPatternW(L"48 8d 0d ? ? ? ? e8 ? ? ? ? 0f b6 83 ? ? ? ?", L"CryLogAlways()");
    void* CryLogAlways_Ptr = (void*)ReadLEA32(CryLogAlways_Addr + 7, L"CryLog()", 0, 1, 5);
    g_System = (ISystem**)ReadLEA32(g_SystemAddr, L"IConsole**", 0, 3, 7);
    CryLogAlways_ = (CryLogAlways__ptr)CryLogAlways_Ptr;
    if (bVerboseConsoleOutput)
    {
        void* CryLogPtr = (void*)ReadLEA32(AfterLogosHook + 7, L"CryLog()", 0, 1, 5);
        Memory::DetourFunction32(CryLogPtr, CryLogAlways_Ptr, 5);
    }
    if (bBenchmarkingOnly)
    {
        if (AfterLogosHook && g_System)
        {
            void* JumpPattern = (void*)FindAndPrintPatternW(L"CC CC CC CC CC CC CC CC CC CC CC CC CC", L"Int3 Arrray");
            CallFunction32((void*)(AfterLogosHook + 7), JumpPattern, 5);
            Memory::DetourFunction64(JumpPattern, (void*)&CRichPresence::SetRichPresence_Hook, 14);
        }
    }
}

DWORD __stdcall Main(void*)
{
    baseModule = GetModuleHandle(NULL);
    ReadConfig();
    DoPatches();
    return true;
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
        CreateThread(NULL, 0, Main, 0, NULL, 0);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
