#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "BF3.Patches"
#define PROJECT_LOG_PATH PROJECT_NAME ".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bLuaPrintToStdOut{};
bool bUnlockConsoleCommands{};

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LOG(L"" PROJECT_NAME " Built: " __TIME__ " @ " __DATE__ "\n");
    LOG(L"" GIT_COMMIT "\n");
    LOG(L"" GIT_VER "\n");
    LOG(L"" GIT_NUM "\n");
    LOG(L"Game Name: %s\n", Memory::GetVersionProductName().c_str());
    LOG(L"Game Path: %s\n", exePath);

    // Initialize config
    // UE4 games use launchers so config path is relative to launcher
    std::wstring config_path = L"" PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.\n");
        std::wstring ini_defaults = L"[Settings]\n"
            wstr(bLuaPrintToStdOut)" = false\n"
            wstr(bUnlockConsoleCommands)" = true\n";
        std::wofstream iniFile(config_path);
        bLuaPrintToStdOut = false;
        bUnlockConsoleCommands = true;
        iniFile << ini_defaults;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bUnlockConsoleCommands), bUnlockConsoleCommands);
        inipp::get_value(ini.sections[L"Settings"], wstr(bLuaPrintToStdOut), bLuaPrintToStdOut);
    }
    LOG(L"%s: 0x%02x\n", wstr(bLuaPrintToStdOut), bLuaPrintToStdOut);
    LOG(L"%s: 0x%02x\n", wstr(bUnlockConsoleCommands), bUnlockConsoleCommands);
}

int LuaPrintToStdOut(void* param_1, char* str)
{
    if (str)
    {
        if (!strcmp(str, "DEBUG") || !strcmp(str, "INFO"))
        {
            printf_s("[%s] ", str);
        }
        else
        {
            printf_s("%s\n", str);
        }
    }
    return 0;
}

bool CallFunction32(void* src, void* dst, int len)
{
    DWORD curProtection{};
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);
    memset(src, 0x90, len);
    uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;
    *(BYTE*)src = 0xE8;
    *(uint32_t*)((uintptr_t)src + 1) = relativeAddress;
    DWORD temp{};
    VirtualProtect(src, len, curProtection, &temp);
    return true;
}

void DisableTAA(void)
{
    if (bLuaPrintToStdOut)
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
        uintptr_t luaPrint = FindAndPrintPatternW(
            L"8d 54 24 10 53 52 e8 ?? ?? ?? ?? 6a fe 56 e8 ?? ?? ?? ?? 47 83 c4 10 3b fd 7e a2",
            L"Lua Print");
        uintptr_t luaPrint2 = FindAndPrintPatternW(
            L"e8 ?? ?? ?? ?? 83 c4 08 6a fe 56 e8 ?? ?? ?? ?? 47 83 c4 08 3b fb",
            L"Lua Print");
        if (luaPrint && luaPrint2)
        {
            CallFunction32((void*)(luaPrint + 6), (void*)LuaPrintToStdOut, 5);
            CallFunction32((void*)luaPrint2, (void*)LuaPrintToStdOut, 5);
        }
    }
    if (bUnlockConsoleCommands)
    {
        const uint8_t patch_flag[] = { 0xba, 0x00, 0x20, 0x00, 0x00, 0x89, 0xd0, 0x90, 0x90 };
        WritePatchPattern(L"66 8b 40 04 ba 00 20 00 00", patch_flag, sizeof(patch_flag), L"Unlock Console Commands", 0);
    }
    printf_s(PROJECT_NAME" - Started.\n");
}

DWORD __stdcall Main()
{
    bLoggingEnabled = false;
    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, L"" PROJECT_LOG_PATH);
    LoggingInit(L"" PROJECT_NAME, LogPath);
    ReadConfig();
    DisableTAA();
    LOG(L"Shutting down " wstr(fp_log) " file handle.\n");
    fclose(fp_log);
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
