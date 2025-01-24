#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bDisableTAA;
bool bDisableDOF;

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
                                    wstr(bDisableTAA)" = true\n"
                                    wstr(bDisableDOF)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableTAA = true;
        bDisableDOF = true;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableDOF), bDisableDOF);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDisableTAA), GetBoolStr(bDisableTAA) , bDisableTAA);
    LOG(L"%s: %s (%i)\n", wstr(bDisableDOF), GetBoolStr(bDisableDOF), bDisableDOF);
}

void DisableTAA(void)
{
    const unsigned char xor_eax_eax[] = { 0x31, 0xc0, 0x90 };
    WritePatchPattern(L"8B 04 8E 85 C0 79 ?? 33 F6 EB ?? BE 06 00 00 00", xor_eax_eax, sizeof(xor_eax_eax), L"Disable TAA", 0);
}

void DisableDOF(void)
{
    const unsigned char xor_edi[] = { 0x31, 0xff, 0x90 };
    WritePatchPattern(L"8B 78 04 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 84 C0 74 ?? 48 8B 03 F7 40 28 00 40 00 00", xor_edi, sizeof(xor_edi), L"Disable Depth of Field", 0);
}

void Main(void)
{
    bLoggingEnabled = false;
    bDisableTAA = false;
    bDisableDOF = false;
    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, _PROJECT_LOG_PATH);
    LoggingInit(_PROJECT_NAME, LogPath);
    ReadConfig();
    if (bDisableTAA)
        DisableTAA();
    if (bDisableDOF)
        DisableDOF();
    LOG(L"Shutting down " wstr(fp_log) " file handle.\n");
    fclose(fp_log);
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
