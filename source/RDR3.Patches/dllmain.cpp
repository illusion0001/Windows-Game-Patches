#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"
#include "main.h"
#include "nativeCaller.h"
#include "natives.h"

#include "mod_enum.h"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "RDR3.Patches"

bool bBenchmarkOnly{};
bool bSkipLegalMainMenu{};

static void ReadConfig()
{
    LOG(L"" GIT_COMMIT "\n");
    LOG(L"" GIT_VER "\n");
    LOG(L"" GIT_NUM "\n");

    inipp::Ini<wchar_t> ini;
    // Initialize config
    std::wstring config_path = L"" PROJECT_NAME ".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        std::wstring ini_defaults = L"[Settings]\n"
            "; Installs No Legal and Main Menu Mod\n"
            "; Mod files:\n"
            "; ```\n"
            "; /x64/data/startup.meta\n"
            "; /x64/main_and_legal_menu_skip_data/legal_screen_patch.ymt\n"
            "; /x64/main_and_legal_menu_skip_data/main_menu_patch.ymt\n"
            "; ```\n"
            "; Source: https://www.rdr2mods.com/downloads/rdr2/other/304-main-and-legal-menu-skip/\n"
            wstr(bSkipLegalMainMenu)" = true\n"
            "; Start benchmark mode after game loads into autosave\n"
            wstr(bBenchmarkOnly)" = false\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bSkipLegalMainMenu = true;
        bBenchmarkOnly = false;
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bSkipLegalMainMenu), bSkipLegalMainMenu);
        inipp::get_value(ini.sections[L"Settings"], wstr(bBenchmarkOnly), bBenchmarkOnly);
    }
    LOG(wstr(bSkipLegalMainMenu) " = %d\n", bSkipLegalMainMenu);
    LOG(wstr(bBenchmarkOnly) " = %d\n", bBenchmarkOnly);
}

static void BranchFunction32(void* src, void* dst, int len)
{
    DWORD curProtection{};
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);
    memset(src, 0xCC /*int3*/, len);
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

#define TYPEDEF_FUNCTION_PTR(ret_type, func_name, ...) \
    typedef ret_type (*func_name##_ptr)(__VA_ARGS__); \
    extern func_name##_ptr func_name;

#define INIT_FUNCTION_PTR(func_name) \
    func_name##_ptr func_name = nullptr

TYPEDEF_FUNCTION_PTR(uint32_t, makeHash_, void* arg1, const char* str);
INIT_FUNCTION_PTR(makeHash_);

typedef struct ScriptFuncArgs_
{
    uint64_t arg[32];
}ScriptFuncArgs;

typedef struct CScriptFuncArgs_
{
    uint64_t* return_value;
    uint64_t arg_count;
    ScriptFuncArgs* Arg;
}CScriptFuncArgs;

TYPEDEF_FUNCTION_PTR(void, CScript_RequestScript, CScriptFuncArgs* arg);
TYPEDEF_FUNCTION_PTR(void, CScript_HasScriptLoaded, CScriptFuncArgs* arg);
TYPEDEF_FUNCTION_PTR(void, CScript_StartNewScript, CScriptFuncArgs* arg);

INIT_FUNCTION_PTR(CScript_RequestScript);
INIT_FUNCTION_PTR(CScript_HasScriptLoaded);
INIT_FUNCTION_PTR(CScript_StartNewScript);

namespace SCRIPTS
{
    void REQUEST_SCRIPT(const char* scriptName) // 0x46ED607DDD40D7FE 0xE26B2666 b1207
    {
        if (!CScript_RequestScript)
        {
            CScript_RequestScript = (CScript_RequestScript_ptr)ReadLEA32(FindAndPrintPatternW(L"48 b9 fe d7 40 dd 7d 60 ed 46", L"CScript_RequestScript") - 7, L"CScript_RequestScript", 0, 3, 7);
        }
        if (CScript_RequestScript)
        {
            static uint64_t ret{};
            static ScriptFuncArgs script_arg{ (uint64_t)scriptName };
            static CScriptFuncArgs arg{ &ret, 1, &script_arg };
            CScript_RequestScript(&arg);
        }
    }
    bool HAS_SCRIPT_LOADED(const char* scriptName) // 0xE97BD36574F8B0A6 0x5D67F751 b1207
    {
        if (!CScript_HasScriptLoaded)
        {
            CScript_HasScriptLoaded = (CScript_HasScriptLoaded_ptr)ReadLEA32(FindAndPrintPatternW(L"48 b9 a6 b0 f8 74 65 d3 7b e9", L"CScript_HasScriptLoaded") - 7, L"CScript_HasScriptLoaded", 0, 3, 7);
        }
        if (CScript_HasScriptLoaded)
        {
            static uint64_t ret{};
            static ScriptFuncArgs script_arg{ (uint64_t)scriptName };
            static CScriptFuncArgs arg{ &ret, 1, &script_arg };
            CScript_HasScriptLoaded(&arg);
            return (bool)arg.return_value[0];
        }
        return false;
    }
    int START_NEW_SCRIPT(const char* scriptName, uint32_t stackSize) // 0xE81651AD79516E48 0x3F166D0E b1207
    {
        if (!CScript_StartNewScript)
        {
            CScript_StartNewScript = (CScript_StartNewScript_ptr)ReadLEA32(FindAndPrintPatternW(L"48 b9 48 6e 51 79 ad 51 16 e8", L"CScript_StartNewScript") - 7, L"CScript_StartNewScript", 0, 3, 7);
        }
        if (CScript_StartNewScript)
        {
            static uint64_t ret{};
            static ScriptFuncArgs script_arg{ (uint64_t)scriptName, (uint64_t)stackSize };
            static CScriptFuncArgs arg{ &ret, 2, &script_arg };
            CScript_StartNewScript(&arg);
            return (int)arg.return_value[0];
        }
        return 0;
    }
}

uint32_t makeHash(const char* str)
{
    return makeHash_(nullptr, str);
}

static uint32_t* AfterLoadScreenHook1(void* arg1, uint32_t* arg2)
{
#ifdef _DEBUG
    AllocConsole();
    SetConsoleTitleA(PROJECT_NAME " - Debug Console");
    freopen_s(reinterpret_cast<FILE**>(stdin), "conin$", "r", stdin);
    freopen_s(reinterpret_cast<FILE**>(stdout), "conout$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stderr), "conout$", "w", stderr);
    ShowWindow(GetConsoleWindow(), SW_SHOW);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hConsole, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, dwMode);
#endif
    * arg2 = 0; // not valid flow
    return arg2;
}
bool breakLoop = false;
static uint32_t* AfterLoadScreenHook2(void* arg1, uint32_t* arg2)
{
    // cannot skip this flow for now, must use mod to workaround it
    // <https://www.rdr2mods.com/downloads/rdr2/other/304-main-and-legal-menu-skip/>
    *arg2 = makeHash("landing_page/flow/landing_launcher_flow");
    return arg2;
}

void Install_NoLogo_MainMenu_Patch(int mode);

uintptr_t CScript_DoScreenFadeInReturn = 0;
uintptr_t CScript_DoScreenFadeOutReturn = 0;

static void __attribute__((naked)) CScript_DoScreenFadeInAsm(CScriptFuncArgs* arg)
{
    __asm
    {
        MOV RAX, qword ptr[RCX + 0x10];
        MOV ECX, dword ptr[RAX];
        jmp[rip + CScript_DoScreenFadeInReturn];
    }
}

static void __attribute__((naked)) CScript_DoScreenFadeOutAsm(CScriptFuncArgs* arg)
{
    __asm
    {
        MOV RAX, qword ptr[RCX + 0x10];
        MOV DL, 0x1;
        jmp[rip + CScript_DoScreenFadeOutReturn];
    }
}

static void SendInputWrapper(WORD inputKey)
{
    INPUT inputs[2]{};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = inputKey;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = inputKey;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput((sizeof(inputs) / sizeof(inputs[0])), inputs, sizeof(INPUT));
}

bool quitProgramAfterCapture = false;

static void DO_SCREEN_FADE_IN_Hook(CScriptFuncArgs* arg)
{
    // https://github.com/Halen84/RDR3-Decompiled-Scripts/blob/5ac4e28d9887ffa934679d559c7cbcda2666fb0c/1491-18/benchmark.c#L1848
    if ((uint32_t)arg->Arg->arg[0] == 1000)
    {
        SendInputWrapper(VK_F11);
        printf_s("Started benchmark\n");
    }
    CScript_DoScreenFadeInAsm(arg);
}

float waitForCaptureProcessing = 0.0f;

static void DO_SCREEN_FADE_OUT_Hook(CScriptFuncArgs* arg)
{
    // https://github.com/Halen84/RDR3-Decompiled-Scripts/blob/5ac4e28d9887ffa934679d559c7cbcda2666fb0c/1491-18/benchmark.c#L3778
    if ((uint32_t)arg->Arg->arg[0] == 1000)
    {
        SendInputWrapper(VK_F11);
        printf_s("Started benchmark\n");
        quitProgramAfterCapture = true;
        waitForCaptureProcessing = 0.0f;
    }
    CScript_DoScreenFadeOutAsm(arg);
}
static void ApplyPatches()
{
    Install_NoLogo_MainMenu_Patch((bSkipLegalMainMenu || bBenchmarkOnly) ? ModInstallEnable : ModDisable);
    if (bBenchmarkOnly)
    {
        baseModule = GetModuleHandle(NULL);
        void* jumpPattern = (void*)FindAndPrintPatternW(L"cc cc cc cc cc cc cc cc cc cc cc cc cc cc", L"Int3 Array");
        void* OpcodePattern = (void*)FindAndPrintPatternW(L"48 83 eb 10 48 ff c7 0f b6 17", L"6c Opcode finder");
        g_nextScrOpcodeReturnAddr = FindAndPrintPatternW(L"4c 8b 75 67 4c 8b 7c 24 38 4c 8b c3 48 ff c7", L"nextScrOpcode", +9);
        makeHash_ = (makeHash__ptr)FindAndPrintPatternW(L"48 63 c1 48 8b ca 4c 8d 04 80 4d 03 c0 48 8d 05 ? ? ? ? 4a ff 24 c0", L"makeHash");
        uintptr_t Cam_DoFadePtrs = FindAndPrintPatternW(L"48 8b 41 10 8b 08 e9 ?? ?? ?? ?? cc 48 8b 41 10 b2 01 8b 08 e9 ?? ?? ?? ??", L"Cam_DoFadePtrs");
        if (OpcodePattern && jumpPattern && g_nextScrOpcodeReturnAddr && Cam_DoFadePtrs)
        {
            BranchFunction32(OpcodePattern, jumpPattern, 10);
            Memory::DetourFunction64(jumpPattern, (void*)Opcode6cAsm, 14);
            {
                void* jumpPattern2 = (void*)FindAndPrintPatternW(L"cc cc cc cc cc cc cc cc cc cc cc cc cc cc", L"Int3 Array");
                BranchFunction32((void*)Cam_DoFadePtrs, jumpPattern2, 6);
                Memory::DetourFunction64(jumpPattern2, (void*)DO_SCREEN_FADE_IN_Hook, 14);
                CScript_DoScreenFadeInReturn = Cam_DoFadePtrs + 6;
            }
            {
                void* jumpPattern2 = (void*)FindAndPrintPatternW(L"cc cc cc cc cc cc cc cc cc cc cc cc cc cc", L"Int3 Array");
                BranchFunction32((void*)(Cam_DoFadePtrs + 12), jumpPattern2, 6);
                Memory::DetourFunction64(jumpPattern2, (void*)DO_SCREEN_FADE_OUT_Hook, 14);
                CScript_DoScreenFadeOutReturn = Cam_DoFadePtrs + 12 + 6;
            }
        }

        // failed attempt at skipping logos and main menu
#ifndef _DEBUG
        if (false)
#endif
        {
            uintptr_t* boot_flow_vftable = (uintptr_t*)ReadLEA32(L"48 8d 05 ? ? ? ? 33 d2 48 89 03 48 8d 8b 30 01 00 00", L"boot_flow_vftable", 0, 3, 7);
            uintptr_t* landing_launcher_flow_vftable = (uintptr_t*)ReadLEA32(L"48 8d 05 ? ? ? ? 48 8b d9 48 89 01 48 81 c1 a8 06 00 00", L"landing_launcher_flow_vftable", 0, 3, 7);
            printf_s("boot_flow_vftable[30] = 0x%p\n", boot_flow_vftable[30]);
            printf_s("landing_launcher_flow_vftable[30] = 0x%p\n", landing_launcher_flow_vftable[30]);

            if (boot_flow_vftable)
            {
                void* jumpPattern2 = (void*)FindAndPrintPatternW(L"cc cc cc cc cc cc cc cc cc cc cc cc cc cc", L"Int3 Array");
                if (jumpPattern2)
                {
                    BranchFunction32((void*)boot_flow_vftable[30], jumpPattern2, 5);
                    Memory::DetourFunction64(jumpPattern2, (void*)AfterLoadScreenHook1, 14);
                }
            }

            /*
            if (landing_launcher_flow_vftable)
            {
                void* jumpPattern2 = (void*)FindAndPrintPatternW(L"cc cc cc cc cc cc cc cc cc cc cc cc cc cc", L"Int3 Array");
                if (jumpPattern2)
                {
                    BranchFunction32((void*)landing_launcher_flow_vftable[30], jumpPattern2, 5);
                    Memory::DetourFunction64(jumpPattern2, (void*)AfterLoadScreenHook2, 14);
                }
            }
            */
        }
    }
}

static void ScriptMain()
{
    bool startedBenchmark = false;
    while (!startedBenchmark)
    {
        if (!startedBenchmark)
        {
            constexpr const char* SCRIPT_NAME = "benchmark";
            constexpr uint32_t SCRIPT_STACK = 1024;
            while (!SCRIPTS::HAS_SCRIPT_LOADED(SCRIPT_NAME))
            {
                SCRIPTS::REQUEST_SCRIPT(SCRIPT_NAME);
                scriptWait(0);
            }
            SCRIPTS::START_NEW_SCRIPT(SCRIPT_NAME, SCRIPT_STACK);
            startedBenchmark = true;
        }
    }
    while (true)
    {
        if (quitProgramAfterCapture)
        {
            if (waitForCaptureProcessing > 4.0f)
            {
                exit(0);
            }
            waitForCaptureProcessing += MISC::GET_SYSTEM_TIME_STEP();
            //printf_s("waitForCaptureProcessing %.1f\n", waitForCaptureProcessing);
        }
        scriptWait(0);
    }
    printf_s("ScriptMain() exit\n");
}

static void Main(HMODULE hModule)
{
    ReadConfig();
    ApplyPatches();
    if (bBenchmarkOnly)
    {
        scriptRegister(hModule, ScriptMain);
    }
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
        Main(hModule);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return 1;
}
