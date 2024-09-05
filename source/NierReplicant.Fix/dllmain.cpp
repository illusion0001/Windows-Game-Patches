#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"NierReplicant.Fix"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bFPSFix{};
float fFPSMax{};

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
            wstr(bFPSFix)" = true\n"
            wstr(fFPSMax)" = 1000\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bFPSFix = true;
        fFPSMax = 1000.f;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bFPSFix), bFPSFix);
        inipp::get_value(ini.sections[L"Settings"], wstr(fFPSMax), fFPSMax);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bFPSFix), GetBoolStr(bFPSFix), bFPSFix);
    LOG(L"%s: %f\n", wstr(fFPSMax), fFPSMax);
}

void Remove30FPSCutscenes()
{
    if (fFPSMax < 1.)
    {
        LOG(L"Cannot set " wstr(fFPSMax) " to a value less than 1!\n");
        LOG(L"Defaulting to 1000.f\n");
        fFPSMax = 1000.f;
    }
    uint8_t* Write30FPSCutscenesResult = Memory::PatternScanW(baseModule, L"89 88 88 3c 00 00 00 a0 aa aa ea 3f 52 b8 1e 85 eb 51 fc 3f");
    uint8_t* Write30FPSCutscenesResult2 = Memory::PatternScanW(baseModule, L"0f 94 c1 e8 ?? ?? ?? ?? 66 0f 6e b7 ?? ?? ?? ??");
    if (Write30FPSCutscenesResult && Write30FPSCutscenesResult2)
    {
        uint64_t Write30FPSAddress = ((uintptr_t)Write30FPSCutscenesResult);
        uint64_t Write30FPSAddress2 = ((uintptr_t)Write30FPSCutscenesResult2);
        float value = (1.f / fFPSMax);
        WritePatchAddressFloat32(Write30FPSAddress, &value, wstr(fFPSMax), 0);
        // remove ugly Sleep
        uint8_t xor_rcx_rcx[] = { 0x48, 0x31, 0xc9 };
        WritePatchAddress(Write30FPSAddress2, xor_rcx_rcx, sizeof(xor_rcx_rcx), L"No sleep", 0);
    }
    else
    {
        LOG(L"Pattern scan failed.\n");
    }
}

DWORD __stdcall Main()
{
    bLoggingEnabled = false;
    bFPSFix = false;
    fFPSMax = 0.;
    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, _PROJECT_LOG_PATH);
    LoggingInit(_PROJECT_NAME, LogPath);
    ReadConfig();
    if (bFPSFix)
        Remove30FPSCutscenes();
    LOG(L"Shutting down " wstr(fp_log) " file handle.\n");
    fclose(fp_log);
    return 0;
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
        Main();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
