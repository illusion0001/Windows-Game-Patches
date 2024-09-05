#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "APT2.Patches"

// INI Variables
bool bDisableTAA;
bool bDisableCA;

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
            wstr(bDisableTAA)" = true\n"
            wstr(bDisableCA)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableTAA = true;
        bDisableCA = true;
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableCA), bDisableCA);
    }
}

bool CreateConsoleHook2(HWND ConsoleWindow)
{
    ShowWindow(ConsoleWindow, 1);
    return 1;
}

void __attribute__((naked)) CreateConsoleHookAsm()
{
    __asm
    {
        MOV RCX, qword ptr[RBX + 0xde00];
        CALL CreateConsoleHook2;
        MOV byte ptr[RBX + 0xdd14], 0x4;
        MOV byte ptr[RBX + 0xdf08], 0x0;
        ADD RSP, 0x30;
        POP RBX;
        RET;
    }
}

#define TYPEDEF_FUNCTION_PTR(ret_type, func_name, ...) \
    typedef ret_type (*func_name##_ptr)(__VA_ARGS__); \
    extern func_name##_ptr func_name;

#define INIT_FUNCTION_PTR(func_name) \
    func_name##_ptr func_name = nullptr

TYPEDEF_FUNCTION_PTR(uint64_t, APT2_Hash_, uint64_t* out, const char* str);
INIT_FUNCTION_PTR(APT2_Hash_);
TYPEDEF_FUNCTION_PTR(void, _RegisterCmd, void* ConsolePtr, const char* CmdName, void* CmdFunc, const char* CmdCmt, void* unk);
INIT_FUNCTION_PTR(_RegisterCmd);

uint64_t APT2_Hash(const char* str, bool showHash)
{
    uint64_t out_temp = 0;
    if (APT2_Hash_)
    {
        APT2_Hash_(&out_temp, str);
        if (showHash)
        {
            wprintf_s(L"%hs -> %.16llx\n", str, out_temp);
        }
    }
    else
    {
        wprintf_s(L"APT2_Hash_ is null\n");
    }
    return out_temp;
}

void RegisterCustomCmd()
{

}

void DisableTAA(void)
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
    WritePatchPattern_Hook(L"c6 83 08 df 00 00 00", 14, L"CreateConsoleHook", 0, (void*)&CreateConsoleHookAsm, nullptr);
    // 40 53 48 83 ec ?? e8 ?? ?? ?? ?? e8 ?? ?? ?? ?? e8 ?? ?? ?? ?? 48 8b 0d
    APT2_Hash_ = (APT2_Hash__ptr)FindAndPrintPatternW(L"48 c7 01 00 00 00 00 4c 8b ca", L"APT2_Hash");
    wprintf_s(L"ReadLEA32: 0x%016llx\n", ReadLEA32(L"40 53 48 83 ec ?? e8 ?? ?? ?? ?? e8 ?? ?? ?? ?? e8 ?? ?? ?? ?? 48 8b 0d", L"_RegisterCmd", 11, 1, 5));
    APT2_Hash("STARTLEVEL", true);
    APT2_Hash("CHECKP_STEALTH_BEEHIVES", true);
    APT2_Hash("CHECKP_NAR_DREAM", true);
    APT2_Hash("CHECKP_NAR_ROADBLOCK", true);
    APT2_Hash("CHECKP_ACTION_MOORS", true);

}

DWORD __stdcall Main(void*)
{
    baseModule = GetModuleHandle(NULL);
    ReadConfig();
    DisableTAA();
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
#ifdef __NEW_THREAD__
        CreateThread(NULL, 0, Main, 0, NULL, 0);
#else
        Main(NULL);
#endif
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
