#include "stdafx.h"
#include "helper.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"BrightMemoryInfinite.NoTAA"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

// INI Variables
bool bDisableTAA;
bool bDisableSharpness;

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LPWSTR exePath = new WCHAR[_MAX_PATH];
    GetModuleFileNameW(baseModule, exePath, _MAX_PATH);
    std::wstring exePathWString(exePath);
    std::wstring wsGameName = Memory::GetVersionProductName();

    LOG(_PROJECT_NAME " Built: " __TIME__ " @ " __DATE__ "\n");
    LOG(L"Game Name: %s\n", wsGameName.c_str());
    LOG(L"Game Path: %s\n", exePathWString.c_str());

    // Initialize config
    // UE4 games use launchers so config path is relative to launcher
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.\n");
        std::wstring ini_defaults = L"[Settings]\n"
                                    wstr(bDisableTAA)" = true\n"
                                    wstr(bDisableSharpness)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableTAA = true;
        bDisableSharpness = true;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableSharpness), bDisableSharpness);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDisableTAA), GetBoolStr(bDisableTAA) , bDisableTAA);
    LOG(L"%s: %s (%i)\n", wstr(bDisableSharpness), GetBoolStr(bDisableSharpness), bDisableSharpness);
}

void DisableTAA(void)
{
    const unsigned char xor_eax_eax[] = { 0x31, 0xc0, 0x90 };
    WritePatchPattern(L"8B 04 8E 83 F8 ?? 77 ?? 89 83 ?? ?? ?? ??", xor_eax_eax, sizeof(xor_eax_eax), L"Disable TAA", 0);
}

void DisableSharpness(void)
{
    const unsigned char xor_xmm8[] = { 0x45, 0x0f, 0x57, 0xc0, 0x90, 0x90 };
    WritePatchPattern(L"F3 44 0F 10 40 04 44 0F 2F C6 73 ?? 45 0F 57 C0 EB ??", xor_xmm8, sizeof(xor_xmm8), L"Disable Sharpness", 0);
}

DWORD __stdcall Main(void*)
{
    bLoggingEnabled = false;
    bDisableTAA = false;
    bDisableSharpness = false;
    LoggingInit(_PROJECT_NAME, _PROJECT_LOG_PATH);
    ReadConfig();
    if (bDisableTAA)
        DisableTAA();
    if (bDisableSharpness)
        DisableSharpness();
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
