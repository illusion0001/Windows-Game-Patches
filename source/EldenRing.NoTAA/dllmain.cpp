#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"EldenRing.NoTAA"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bForceTAAOff;
bool bDisableCA;

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LOG(_PROJECT_NAME " Built: " __TIME__ " @ " __DATE__ "\n");
    LOG(L"Game Name: %s\n", Memory::GetVersionProductName().c_str());
    LOG(L"Game Path: %s\n", exePath);

    // Initialize config
    // UE4 games use launchers so config path is relative to launcher
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.\n");
        std::wstring ini_defaults = L"[Settings]\n"
                                    wstr(bForceTAAOff)" = true\n"
                                    wstr(bDisableCA)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bForceTAAOff = true;
        bDisableCA = true;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bForceTAAOff), bForceTAAOff);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableCA), bDisableCA);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bForceTAAOff), GetBoolStr(bForceTAAOff) , bForceTAAOff);
    LOG(L"%s: %s (%i)\n", wstr(bDisableCA), GetBoolStr(bDisableCA), bDisableCA);
}

void ForceTAAOff(void)
{
    const unsigned char patch[] = { 0x33, 0xD2, 0x90 }; 
    // E8 4FE0C8FF | call eldenring.7FF699AB3D30
    // 8B53 28     | mov edx, dword ptr ds : [rbx + 28]         --> 33D2 | xor edx, edx
    //                                                              90   | nop
    // 48 8BCF     | mov rcx, rdi
    WritePatchPattern(L"E8 ?? ?? ?? ?? 8B 53 28 48 8B CF", patch, sizeof(patch), L"Force TAA Off", 5);
}

void DisableCA(void)
{
    const unsigned char patch[] = { 0x33, 0xC0, 0x90 };
    // 8B42 38       | mov eax, dword ptr ds : [rdx + 38]       --> 33C0 | xor eax, eax
    //                                                              90   | nop
    // 8981 DC000000 | mov dword ptr ds : [rcx + DC] , eax
    WritePatchPattern(L"8B 42 38 89 81 DC 00 00 00", patch, sizeof(patch), L"Disable CA", 0);
}

DWORD __stdcall Main(void*)
{
    bLoggingEnabled = false;

    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, L"" _PROJECT_LOG_PATH);
    LoggingInit(L"" _PROJECT_NAME, LogPath);
    ReadConfig();

    if (bForceTAAOff)
        ForceTAAOff();
    if (bDisableCA)
        DisableCA();

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
