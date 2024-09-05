#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"GotG.NoTAA"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bDisableTAA;

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LOG(_PROJECT_NAME " Built: " __TIME__ " @ " __DATE__ "\n");
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
                                    wstr(bDisableTAA)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableTAA = true;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDisableTAA), GetBoolStr(bDisableTAA), bDisableTAA);
}

void DisableTAA(void)
{
    // 0000000140979FE0 | 803D 2F016E02 00 | cmp byte ptr ds : [14305A116] , 0          --> C605 2F016E02 00 | mov byte ptr ds : [14305A116] , 0
    // 0000000140979FE7 | 4C:8BD2          | mov r10, rdx 
    // 0000000140979FEA | 4C:8BC9          | mov r9, rcx 
    // 0000000140979FED | 0F84 C5000000    | je gotg.14097A0B8                          --> C3 | ret
    //                                                                                      90 | nop
    //                                                                                      90 | nop
    //                                                                                      90 | nop
    //                                                                                      90 | nop
    //                                                                                      90 | nop

    const unsigned char patch_mov[] = { 0xC6, 0x05 };
    const unsigned char patch_ret[] = { 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90 };

    uintptr_t patch_address = FindAndPrintPatternW(L"80 3D ?? ?? ?? ?? 00 4C 8B D2 4C 8B C9 0F 84 C5 ?? ?? ??", L"Projection matrix jitter sub", 0);
    WritePatchAddress(patch_address, patch_mov, sizeof(patch_mov), L"Disable TAA", 0);
    WritePatchAddress(patch_address, patch_ret, sizeof(patch_ret), L"Disable TAA", 13);
}

DWORD __stdcall Main(void*)
{
    bLoggingEnabled = false;
    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, _PROJECT_LOG_PATH);
    LoggingInit(_PROJECT_NAME, LogPath);
    ReadConfig();

    if (bDisableTAA)
        DisableTAA();

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
