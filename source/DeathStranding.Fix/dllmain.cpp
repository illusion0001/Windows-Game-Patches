#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"DeathStranding.Fix"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
uint32_t uForceAAMode;
bool bSkipSavegameChecks;

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
    // UE4 games use launchers so config path is relative to launcher
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.\n");
        std::wstring ini_defaults = L"[Settings]\n"
                                    wstr(uForceAAMode)" = 0\n"
                                    "; input corresponding numbers below\n"
                                    "; Valid AA setting:\n"
                                    "; None = 0\n"
                                    "; FXAA = 1\n"
                                    "; TAA = 2\n"
                                    wstr(bSkipSavegameChecks)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        uForceAAMode = 0;
        bSkipSavegameChecks = true;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(uForceAAMode), uForceAAMode);
        inipp::get_value(ini.sections[L"Settings"], wstr(bSkipSavegameChecks), bSkipSavegameChecks);
    }

    // Log config parse
    LOG(L"%s: %u\n", wstr(uForceAAMode), uForceAAMode);
    LOG(L"%s: %s (%i)\n", wstr(bSkipSavegameChecks), GetBoolStr(bSkipSavegameChecks), bSkipSavegameChecks);
}

void ForceAAMode(void)
{
    // 83FB 02       | cmp ebx, 2                               
    // 0F46C3        | cmovbe eax, ebx
    // 8905 455D3D02 | mov dword ptr ds : [7FF63D0F500C] , eax

    uintptr_t mov_AA_eax = FindAndPrintPatternW(L"83 ?? 02 0F 46 ?? 89 05 ?? ?? ?? ??", L"Find init AA sub", 6);
    // mov_AA_eax = address of "mov dword ptr ds : [7FF63D0F500C] , eax",
    uint32_t* AA_offset = reinterpret_cast<uint32_t*>(mov_AA_eax + 2);
    // *AA_offset = BE(0x455D3D02) = 0x023D5D45
    uint32_t* AA_setting = reinterpret_cast<uint32_t*>(mov_AA_eax + 6 + *AA_offset);
    // AA_setting = address of AA setting, that is: sum of end of mov function (mov_AA_eax + 6) + mov offset (AA_offset) = 0x7FF63D0F500C
    LOG(L"Offset of mov function: 0x%08llx (at address 0x%016llx)\n", *AA_offset, AA_offset);
    LOG(L"Current AA setting: 0x%08llx (at address 0x%016llx)\n", *AA_setting, AA_setting);
    *AA_setting = uForceAAMode;
    LOG(L"Set new AA setting: 0x%08llx (at address 0x%016llx)\n", *AA_setting, AA_setting);
}

void SkipSavegameChecks(void)
{
    const unsigned char patch[] = { 0xEB, 0x05 };
    // 84DB          | test bl, bl
    // 75 05         | jne ds.7FF72EADEB8C                      --> EB 05        | jmp ds.7FF72EADEB8C
    // 41:C646 3A 11 | mov byte ptr ds : [r14 + 3A] , 11
    WritePatchPattern(L"84 DB 75 05 41 C6 46 3A 11", patch, sizeof(patch), L"Skip Savegame Checks", 2);
}

DWORD __stdcall Main(void*)
{
    bLoggingEnabled = false;

    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, L"" _PROJECT_LOG_PATH);
    LoggingInit(L"" _PROJECT_NAME, LogPath);
    ReadConfig();

    ForceAAMode();
    if (bSkipSavegameChecks)
        SkipSavegameChecks();

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
