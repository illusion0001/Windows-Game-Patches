#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bRemove30FPSCutscenes;
bool bDisableTAA;

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
                                    "; " wstr(bRemove30FPSCutscenes) " causes double speed, needs more research.\n"
                                    wstr(bRemove30FPSCutscenes)" = false\n"
                                    wstr(bDisableTAA)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bRemove30FPSCutscenes = false;
        bDisableTAA = true;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bRemove30FPSCutscenes), bRemove30FPSCutscenes);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bRemove30FPSCutscenes), GetBoolStr(bRemove30FPSCutscenes) , bRemove30FPSCutscenes);
    LOG(L"%s: %s (%i)\n", wstr(bDisableTAA), GetBoolStr(bDisableTAA), bDisableTAA);
}

void Remove30FPSCutscenes(void)
{
    uint8_t* Write30FPSCutscenesResult = Memory::PatternScanW(baseModule, L"C7 80 ?? ?? ?? ?? 1E 00 00 00 48 8B ?? ?? ?? ?? ?? C7 80 ?? ?? ?? ?? 1E 00 00 00 48 8B 5D 00 48 85 DB");
    if (Write30FPSCutscenesResult)
    {
        const unsigned char mov_ecx[] = { 0x89, 0x88 };
        const unsigned char nop4x[] = { 0x90, 0x90, 0x90, 0x90 };
        uint64_t Write30FPSAddress = ((uintptr_t)Write30FPSCutscenesResult);
        WritePatchAddress(Write30FPSAddress, mov_ecx, sizeof(mov_ecx), wstr(Remove30FPSCutscenes), 0);
        WritePatchAddress(Write30FPSAddress, mov_ecx, sizeof(mov_ecx), wstr(Remove30FPSCutscenes), 17);
        WritePatchAddress(Write30FPSAddress, nop4x, sizeof(nop4x), wstr(Remove30FPSCutscenes), 6);
        WritePatchAddress(Write30FPSAddress, nop4x, sizeof(nop4x), wstr(Remove30FPSCutscenes), 23);
    }
    else
    {
        LOG(L"Pattern scan failed.\n");
    }
}

void DisableTAA(void)
{
    // Still has sharpening filter
    const unsigned char xor_r8d[] = { 0x45, 0x31, 0xc0 };
    WritePatchPattern(L"44 8B 00 45 85 C0 0F 9F C0 88 41 10 45 85 C0 0F 8E ?? ?? ?? ?? 4C 8B 81 00 01 00 00", xor_r8d, sizeof(xor_r8d), wstr(DisableTAA), 0);
}

DWORD __stdcall Main(void*)
{
    bLoggingEnabled = false;
    bRemove30FPSCutscenes = false;
    bDisableTAA = false;
    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, _PROJECT_LOG_PATH);
    LoggingInit(_PROJECT_NAME, LogPath);
    ReadConfig();
    if (bRemove30FPSCutscenes)
        Remove30FPSCutscenes();
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
