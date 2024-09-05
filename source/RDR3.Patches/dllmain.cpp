#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "main.h"
#include "nativeCaller.h"
#include "natives.h"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "RDR3.Patches"

bool bBenchmarkOnly{};
bool bSkipLegalMainMenu{};

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
            "; Thanks to this mod for reference on flowblock: https://www.rdr2mods.com/downloads/rdr2/other/304-main-and-legal-menu-skip/\n"
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
        cmp dword ptr[rcx + 0x1090], 5;
        jz  not_match;
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

void showConsole()
{
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
}

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

uintptr_t ParseFlowBlockReturn = 19;

typedef struct xml_keys_
{
    void* not_useful[9];
    char* key;
    void* not_useful2[3];
}xml_keys;

static void __attribute__((naked)) ParseFlowBlockAsm(void* arg1, xml_keys* keys, const char* prev_key)
{
    __asm
    {
        MOV RAX, RSP;
        MOV qword ptr[RAX + 0x8], RBX;
        MOV qword ptr[RAX + 0x10], RBP;
        MOV qword ptr[RAX + 0x18], RSI;
        MOV qword ptr[RAX + 0x20], RDI;
        jmp qword ptr[rip + ParseFlowBlockReturn];
    }
}

// https://github.com/PLTytus/various_joaat/blob/e06f51b1cfe95c2690b0fb46adce51c7d1f81d8a/joaat.c
// Hash method doesn't really matter
// Its just to prevent using strcmp
constexpr uint32_t joaat(const char* s)
{
    uint32_t hash = 0;
    for (; *s; ++s)
    {
        hash += *s;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

static int detect_count = 0;

static void ParseFlowBlockHook(void* arg1, xml_keys* keys, const char* prev_key)
{
    uint32_t key_hash = 0;
    if (!keys->key || detect_count == 2)
    {
        // the compiler will generate similar code if i used call() + return
        goto no_match_or_quit;
    }
    key_hash = joaat(keys->key);
    switch (key_hash)
    {
    case joaat("feedback_context_switch.boot_screen_host.sign_in_flow_activity_sentinel.account_picker_activity_sentinel.account_picker_wrapper"):
    {
        keys->key = (char*)"feedback_context_switch.boot_screen_host.profile_flow_activity_sentinel.wait_for_profile";
        detect_count++;
        break;
    }
    case joaat("input_context_switch.can_access_sp_check"):
    case joaat("input_context_switch.boot_flow_startup"):
    case joaat("input_context_switch.boot_flow_invite"):
    case joaat("input_context_switch.boot_flow_sign_out"):
    case joaat("input_context_switch.mp_direct_dispatch_code_event.mp_direct_scene.mp_direct_host"):
    {
        keys->key = (char*)"input_context_switch.landing_camera_fader.landing_main.landing_filter.priority_feed_decision_sp.set_feedback_context.from_priority_feed_decision_sp_can_access_sp.sp_menu.launch_sp";
        break;
    }
    default:
    {
        break;
    }
    }
no_match_or_quit:
    ParseFlowBlockAsm(arg1, keys, prev_key);
}

static void ApplyPatches()
{
    if (bBenchmarkOnly)
    {
        void* jumpPattern = (void*)FindAndPrintPatternW(L"cc cc cc cc cc cc cc cc cc cc cc cc cc cc", L"Int3 Array");
        void* OpcodePattern = (void*)FindAndPrintPatternW(L"48 83 eb 10 48 ff c7 0f b6 17", L"6c Opcode finder");
        g_nextScrOpcodeReturnAddr = FindAndPrintPatternW(L"4c 8b 75 67 4c 8b 7c 24 38 4c 8b c3 48 ff c7", L"nextScrOpcode", +9);
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
    }

    // generates cleaner diff
    {
        if (bBenchmarkOnly || bSkipLegalMainMenu)
        {
            uintptr_t ParseXmlBlockAddr = FindAndPrintPatternW(L"48 8b c4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 ec 20 49 8b f0 48 8b da 4c 8b 41 08", L"ParseXmlBlockAddr");
            uintptr_t BogusErrorCheck = FindAndPrintPatternW(L"48 83 ec 28 45 33 c9 e8 ?? ?? ?? ?? cc", L"BogusErrorCheck");
            if (ParseXmlBlockAddr && BogusErrorCheck)
            {
                ParseFlowBlockReturn += ParseXmlBlockAddr;
                Memory::DetourFunction64((void*)ParseXmlBlockAddr, (void*)ParseFlowBlockHook, 14);
                // patches sus errors when replacing the pointer in `ParseFlowBlockHook`
                // TODO: Check if calling C script from a hook works now
                // and also find a c script function that only runs when player is alive and once a frame
                // this will hopefully eliminate the need for linking with scriptHook
                // just like orbis version
                unsigned char ret[] = { 0xc3 };
                Memory::PatchBytes(BogusErrorCheck, ret, sizeof(ret));
            }
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
#ifdef _DEBUG
    showConsole();
#endif
    baseModule = GetModuleHandle(NULL);
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
