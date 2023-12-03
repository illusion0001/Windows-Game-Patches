#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"RE8.NoTAA"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bDisableSharpnessOnly;
bool bDisableTAA;

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LOG(_PROJECT_NAME " Built: " __TIME__ " @ " __DATE__ "\n");
    LOG(L"" GIT_COMMIT "\n");
    LOG(L"" GIT_VER "\n");
    LOG(L"" GIT_NUM "\n");
    LOG(L"Game Name: %s\n", Memory::GetVersionProductName().c_str());
    LOG(L"Game Path: %s\n", exePath);

    // Initialize config
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.\n");
        std::wstring ini_defaults = L"[Settings]\n"
                                    wstr(bDisableTAA)" = true\n"
                                    wstr(bDisableSharpnessOnly)" = false\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableTAA = true;
        bDisableSharpnessOnly = false;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableSharpnessOnly), bDisableSharpnessOnly);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDisableTAA), GetBoolStr(bDisableTAA), bDisableTAA);
    LOG(L"%s: %s (%i)\n", wstr(bDisableSharpnessOnly), GetBoolStr(bDisableSharpnessOnly) , bDisableSharpnessOnly);
}

void DisableSharpnessOnly(void)
{
    // 00007FF71DF983ED | 80B8 5A010000 00 | cmp byte ptr ds:[rax+15A],0        | --> C680 5A010000 01 | mov byte ptr ds:[rax+15A],1
    // 00007FF71DF983F4 | 75 49            | jne re8.7FF71DF9843F               | --> EB 49            | jmp re8.7FF71DF9843F
    // 00007FF71DF983F6 | 83B8 F8010000 05 | cmp dword ptr ds : [rax + 1F8] , 5 |
    // 00007FF71DF983FD | 74 40            | je re8.7FF71DF9843F                |
    const unsigned char patch[] = { 0xC6, 0x80, 0x5A, 0x01, 0x00, 0x00, 0x01, 0xEB, 0x49 };
    WritePatchPattern(L"80 B8 5A 01 00 00 00 75 49", patch, sizeof(patch), L"Disable Sharpness", 0);
}

void DisableTAA(void)
{
    // 00007FF71DF983ED | 80B8 5A010000 00 | cmp byte ptr ds:[rax+15A],0        |
    // 00007FF71DF983F4 | 75 49            | jne re8.7FF71DF9843F               |     
    // 00007FF71DF983F6 | 83B8 F8010000 05 | cmp dword ptr ds : [rax + 1F8] , 5 | --> C680 F8010000 05 | mov byte ptr ds : [rax + 1F8] , 5
    // 00007FF71DF983FD | 74 40            | je re8.7FF71DF9843F                | --> EB 40            | jmp re8.7FF71DF9843F |
    const unsigned char patch[] = { 0xC6, 0x80, 0xF8, 0x01, 0x00, 0x00, 0x05, 0xEB, 0x40 };
    WritePatchPattern(L"83 B8 F8 01 00 00 05 74 40", patch, sizeof(patch), L"Disable TAA", 0);
}

DWORD __stdcall Main(void*)
{
    bLoggingEnabled = false;
    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, _PROJECT_LOG_PATH);
    LoggingInit(_PROJECT_NAME, LogPath);
    ReadConfig();

    // Enforce TAA patch, as it already disables the sharpening filter
    if (bDisableTAA)
        DisableTAA();
    else
    {
        if (bDisableSharpnessOnly)
            DisableSharpnessOnly();
    }

    LOG(L"Shutting down " wstr(fp_log) " file handle.\n");
    fclose(fp_log);
    return true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
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
