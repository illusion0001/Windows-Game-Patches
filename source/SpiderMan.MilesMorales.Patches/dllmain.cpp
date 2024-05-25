#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "SpiderMan.MilesMorales.Patches"

// INI Variables
bool bDisableStartupLogo{};
bool bEnableConsole{};

void ReadConfig()
{
    inipp::Ini<wchar_t> ini;
    // Initialize config
    std::wstring config_path = L"" PROJECT_NAME ".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        std::wstring ini_defaults = L"[Settings]\n"
            wstr(bDisableStartupLogo)" = true\n"
            wstr(bEnableConsole)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableStartupLogo = true;
        bEnableConsole = true;
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableStartupLogo), bDisableStartupLogo);
        inipp::get_value(ini.sections[L"Settings"], wstr(bEnableConsole), bEnableConsole);
    }
    LOG(wstr(bDisableStartupLogo) " = %d\n", bDisableStartupLogo);
    LOG(wstr(bEnableConsole) " = %d\n", bEnableConsole);
}

void ShowConsole()
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

void ApplyPatches()
{
    if (bDisableStartupLogo)
    {
        uint32_t boot_mode = 2; // boot mode: main menu
        WritePatchPattern_Int(sizeof(boot_mode), L"c7 43 0c 01 00 00 00 48 8d 0d ?? ?? ?? ?? 66 c7 43 10 00 00", (void*)boot_mode, L"Skip Startup Logos", 3);
    }
    if (bEnableConsole)
    {
#ifdef _DEBUG
#else
        ShowConsole();
#endif
        // 48 89 54 24 10 33 c0 4c 89 44 24 18 4c 89 4c 24 20 c3
        WritePatchPattern_Hook(L"48 89 54 24 10 33 c0 4c 89 44 24 18 4c 89 4c 24 20 c3", 14, L"Enable Console", 0, (void*)printf_s, nullptr);
    }
}

DWORD Main()
{
    baseModule = GetModuleHandle(NULL);
#ifdef _DEBUG
    ShowConsole();
#endif
    ReadConfig();
    ApplyPatches();
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
        Main();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
