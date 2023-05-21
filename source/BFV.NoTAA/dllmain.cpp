#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "BFV.NoTAA"
#define PROJECT_LOG_PATH PROJECT_NAME ".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bDisableTAA;
uint32_t uInjection_Delay;
uint32_t uPostAA_Mode;

enum PostAA_Type
{
    PostProcessAAMode_None = 0,
    PostProcessAAMode_FxaaLow = 1,
    PostProcessAAMode_FxaaMedium = 2,
    PostProcessAAMode_FxaaHigh = 3,
    PostProcessAAMode_FxaaCompute = 4,
    PostProcessAAMode_FxaaComputeExtreme = 5,
    PostProcessAAMode_Smaa1x = 6,
    PostProcessAAMode_SmaaT2x = 7,
    PostProcessAAMode_TemporalAA = 8
};

const wchar_t* GetPostAA_Mode(uint32_t mode)
{
    switch (mode)
    {
    case PostProcessAAMode_None:
        return L"PostProcessAAMode_None";
    case PostProcessAAMode_FxaaLow:
        return L"PostProcessAAMode_FxaaLow";
    case PostProcessAAMode_FxaaMedium:
        return L"PostProcessAAMode_FxaaMedium";
    case PostProcessAAMode_FxaaHigh:
        return L"PostProcessAAMode_FxaaHigh";
    case PostProcessAAMode_FxaaCompute:
        return L"PostProcessAAMode_FxaaCompute";
    case PostProcessAAMode_FxaaComputeExtreme:
        return L"PostProcessAAMode_FxaaComputeExtreme";
    case PostProcessAAMode_Smaa1x:
        return L"PostProcessAAMode_Smaa1x";
    case PostProcessAAMode_SmaaT2x:
        return L"PostProcessAAMode_SmaaT2x";
    case PostProcessAAMode_TemporalAA:
        return L"PostProcessAAMode_TemporalAA";
    default:
        return L"Unknown";
    }
}

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
                                    wstr(uInjection_Delay)" = 2 ; wait amount of seconds for patch\n"
                                    wstr(bDisableTAA)" = true\n"
                                    wstr(uPostAA_Mode)" = 0\n"
                                    "; input corresponding numbers below\n"
                                    "; Valid PostAA Mode:\n"
                                    "; PostProcessAAMode_None = 0\n"
                                    "; PostProcessAAMode_FxaaLow = 1\n"
                                    "; PostProcessAAMode_FxaaMedium = 2\n"
                                    "; PostProcessAAMode_FxaaHigh = 3\n"
                                    "; PostProcessAAMode_FxaaCompute = 4\n"
                                    "; PostProcessAAMode_FxaaComputeExtreme = 5\n"
                                    "; PostProcessAAMode_Smaa1x = 6\n"
                                    "; PostProcessAAMode_SmaaT2x = 7\n"
                                    "; PostProcessAAMode_TemporalAA = 8\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        uInjection_Delay = 2;
        bDisableTAA = true;
        uPostAA_Mode = 0;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(uInjection_Delay), uInjection_Delay);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
        inipp::get_value(ini.sections[L"Settings"], wstr(uPostAA_Mode), uPostAA_Mode);
    }

    // Log config parse
    LOG(L"%s: %u seconds\n", wstr(uInjection_Delay), uInjection_Delay);
    LOG(L"%s: %s (%u)\n", wstr(bDisableTAA), GetBoolStr(bDisableTAA) , bDisableTAA);
    LOG(L"%s: %s (%u)\n", wstr(uPostAA_Mode), GetPostAA_Mode(uPostAA_Mode), uPostAA_Mode);
}

void DisableTAA(void)
{
    if (uPostAA_Mode == PostProcessAAMode_TemporalAA)
    {
        LOG(L"Using PostAA Mode %u (%s). No changes made.\n", uPostAA_Mode, GetPostAA_Mode(uPostAA_Mode));
        return;
    }
    unsigned char PostAA_Mode0[] = { 0xC6, 0x87, 0xE8, 0x02, 0x00, 0x00, 0x00, 0xEB, 0x1D };
    if (uPostAA_Mode >= 0 && uPostAA_Mode <= 8)
    {
        PostAA_Mode0[6] = (uint8_t)uPostAA_Mode;
    }
    else
    {
        uPostAA_Mode = 0;
        PostAA_Mode0[6] = (uint8_t)uPostAA_Mode;
        LOG(L"Invalid PostAA mode! %u (%s)", uPostAA_Mode, GetPostAA_Mode(uPostAA_Mode));
        LOG(L"Using default post AA mode of 0: %s", GetPostAA_Mode(uPostAA_Mode));
    }
    WritePatchPattern(L"83 BF E8 02 00 00 08 75 ??", PostAA_Mode0, sizeof(PostAA_Mode0), L"Disable TAA", 0);
}

DWORD __stdcall Main(void*)
{
    bLoggingEnabled = false;
    uInjection_Delay = 0;
    bDisableTAA = false;
    uPostAA_Mode = 0;
    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, L"" PROJECT_LOG_PATH);
    LoggingInit(L"" PROJECT_NAME, LogPath);
    ReadConfig();
    if (uInjection_Delay > 0)
        Sleep(uInjection_Delay * 1000);
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
