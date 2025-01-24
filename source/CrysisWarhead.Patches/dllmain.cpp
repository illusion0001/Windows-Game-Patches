#include "stdafx.h"
#include "helper.hpp"
#define MAX_CAVE_SIZE 128
#include "memory.hpp"

#include "function_ptr.h"

TYPEDEF_FUNCTION_PTR(void, RegisterCmd_original, void* this_, const char* cmd, void* func, int flags, const char* desc);
INIT_FUNCTION_PTR(RegisterCmd_original);

TYPEDEF_FUNCTION_PTR(void, CryLogAlways, const char* fmt, ...);
INIT_FUNCTION_PTR(CryLogAlways);

HMODULE baseModule{}, CrySystemModule{};

uint8_t* DAT_377bb956 = 0;

static void cmd_Chickens()
{
    bool enable = *DAT_377bb956 == 0x7f;
    *DAT_377bb956 = enable ? 0xff : 0x7f;
    CryLogAlways("Chickens %s\n", enable ? "enabled" : "disabled");
    LOG("Chickens %hs\n", enable ? "enabled" : "disabled");
}

static void RegisterCmdHook(void* this_, const char* cmd, void* func, int flags, const char* desc)
{
    LOG("Command: %hs -> 0x%p flag 0x%x\n", cmd, func, flags);
    static bool once{};
    if (!once)
    {
        // non 0x80000 for devmode
        RegisterCmd_original(this_, "chickens", (void*)&cmd_Chickens, 0x80000, "Enable chicken anti piracy measure");
        once = true;
    }
    RegisterCmd_original(this_, cmd, func, flags, desc);
}

static void DoPatches()
{
    const uintptr_t RegisterCmd = FindAndPrintPatternW(CrySystemModule, L""
                                                                        "44 89 4C 24 20 "
                                                                        "4C 89 44 24 18 "
                                                                        "48 89 4C 24 08 "
                                                                        "53 "
                                                                        "55 "
                                                                        "56 "
                                                                        "57 "
                                                                        "41 54 "
                                                                        "41 55 "
                                                                        "41 56 "
                                                                        "41 57 "
                                                                        "48 83 EC 58 "
                                                                        "48 85 D2", L"CrySystem::RegisterCmd");
    if (RegisterCmd)
    {
        const size_t RegisterCmdCaveSize = 16;
        const uintptr_t RegisterCmdCave = Memory::CreatePrologueHook(RegisterCmd, RegisterCmdCaveSize);
        if (RegisterCmdCave)
        {
            RegisterCmd_original = (RegisterCmd_original_ptr)RegisterCmdCave;
            Memory::DetourFunction64(RegisterCmd, (uintptr_t)&RegisterCmdHook, RegisterCmdCaveSize);
        }
    }
    const uintptr_t drmPtr = FindAndPrintPatternW(L"83 f8 20 7e ? 0f b6 05 ? ? ? ? 88 05 ? ? ? ?", L"drmPtr", 12);
    if (drmPtr)
    {
        DAT_377bb956 = (uint8_t*)ReadLEA32(drmPtr, L"DAT_377bb956", 0, 2, 6);
        LOG("DAT_377bb956: 0x%p\n", DAT_377bb956);
    }
    const uintptr_t CryLogAlwaysAddr = FindAndPrintPatternW(CrySystemModule,    L""
                                                                                "48 89 4C 24 08 "
                                                                                "48 89 54 24 10 "
                                                                                "4C 89 44 24 18 "
                                                                                "4C 89 4C 24 20 "
                                                                                "48 83 EC 28 "
                                                                                "4C 8B C1 "
                                                                                "48 8B 0D ? ? ? ? "
                                                                                "48 85 C9 "
                                                                                "74 ?", L"CryLogAlwaysAddr");
    if (CryLogAlwaysAddr)
    {
        CryLogAlways = (CryLogAlways_ptr)CryLogAlwaysAddr;
    }
}

static BOOL Main()
{
    // by the time `version.dll` is loaded, c1-launcher has already done its crt startup for `Crysis64.exe`.
    baseModule = GetModuleHandle(L"Crysis64.exe");
    CrySystemModule = GetModuleHandle(L"CrySystem.dll");
    if (baseModule && CrySystemModule)
    {
        if (IsDebuggerPresent())
        {
            ShowConsole1();
        }
        LOG("Crysis64.exe: 0x%llx\n", baseModule);
        LOG("CrySystem.dll: 0x%llx\n", CrySystemModule);
        DoPatches();
        return true;
    }
    return false;
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
            return Main();
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
