#include "stdafx.h"
#include "helper.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"RE4.Sharpness"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

// INI Variables
bool bRemoveSharpness;

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
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.\n");
        std::wstring ini_defaults = L"[Settings]\n"
                                    wstr(bRemoveSharpness)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bRemoveSharpness = true;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bRemoveSharpness), bRemoveSharpness);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bRemoveSharpness), GetBoolStr(bRemoveSharpness) , bRemoveSharpness);
}

void RemoveSharpness(void)
{
    const unsigned char patch1[] = { 0x31, 0xd2, 0x90 };
    const unsigned char patch2[] = { 0x0F, 0x57, 0xC0, 0x90, 0x90 };
    WritePatchPattern(L"8B 56 28 48 8B CB E8 ?? ?? ?? ??", patch1, sizeof(patch1), L"TAA Flag", 0);
    WritePatchPattern(L"F3 0F 10 46 3C 48 8B CB 0F 5A C0 66 0F 5A C8 E8 ?? ?? ?? ??", patch2, sizeof(patch2), L"Sharpness", 0);
}

DWORD __stdcall Main(void*)
{
    Sleep(5 * 1000); // Bypass the checksum protection
    bLoggingEnabled = false;
    bRemoveSharpness = false;
    LoggingInit(_PROJECT_NAME, _PROJECT_LOG_PATH);
    ReadConfig();
    if (bRemoveSharpness)
        RemoveSharpness();
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
