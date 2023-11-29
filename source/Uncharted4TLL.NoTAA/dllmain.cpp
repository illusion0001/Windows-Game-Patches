#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"
#include "code_caves.hpp"
#include "patterns.hpp"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"Uncharted4TLL.NoTAA"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

wchar_t exePath[_MAX_PATH] = { 0 };
wchar_t u4_exe[] = { L"u4.exe" };
wchar_t u4_l_exe[] = { L"u4-l.exe" };
wchar_t tll_exe[] = { L"tll.exe" };
wchar_t tll_l_exe[] = { L"tll-l.exe" };

// INI Variables
bool bDebugMenu{};
bool bSkipIntroLogos{};
bool bDisableTAA{};
bool bDisableSharpening{};
bool bDisableBarrelDistortion{};
bool bDisableScreenZoom{};
bool bDisableChromaticAberration{};
bool bDisableDoF{};
bool bDisableVignette{};

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
        std::wstring ini_defaults = L""
            "[Settings]\n"
            "; Debug Menu by SunBeam\n"
            wstr(bDebugMenu)" = false\n"
            "; Skip Startup Logo Videos\n"
            wstr(bSkipIntroLogos)" = true\n"
            wstr(bDisableTAA)" = true\n"
            wstr(bDisableSharpening)" = true\n"
            wstr(bDisableBarrelDistortion)" = true\n"
            wstr(bDisableScreenZoom)" = true\n"
            wstr(bDisableChromaticAberration)" = true\n"
            wstr(bDisableDoF)" = true\n"
            wstr(bDisableVignette)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDebugMenu = false;
        bSkipIntroLogos = true;
        bDisableTAA = true;
        bDisableSharpening = true;
        bDisableBarrelDistortion = true;
        bDisableScreenZoom = true;
        bDisableChromaticAberration = true;
        bDisableDoF = true;
        bDisableVignette = true;

        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDebugMenu),                   bDebugMenu);
        inipp::get_value(ini.sections[L"Settings"], wstr(bSkipIntroLogos),              bSkipIntroLogos);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA),                  bDisableTAA);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableSharpening),           bDisableSharpening);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableBarrelDistortion),     bDisableBarrelDistortion);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableScreenZoom),           bDisableScreenZoom);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableChromaticAberration),  bDisableChromaticAberration);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableDoF),                  bDisableDoF);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableVignette),             bDisableVignette);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDebugMenu),                     GetBoolStr(bDebugMenu),                     bDebugMenu);
    LOG(L"%s: %s (%i)\n", wstr(bSkipIntroLogos),                GetBoolStr(bSkipIntroLogos),                bSkipIntroLogos);
    LOG(L"%s: %s (%i)\n", wstr(bDisableTAA),                    GetBoolStr(bDisableTAA),                    bDisableTAA); 
    LOG(L"%s: %s (%i)\n", wstr(bDisableSharpening),             GetBoolStr(bDisableSharpening),             bDisableSharpening);
    LOG(L"%s: %s (%i)\n", wstr(bDisableBarrelDistortion),       GetBoolStr(bDisableBarrelDistortion),       bDisableBarrelDistortion);
    LOG(L"%s: %s (%i)\n", wstr(bDisableScreenZoom),             GetBoolStr(bDisableScreenZoom),             bDisableScreenZoom);
    LOG(L"%s: %s (%i)\n", wstr(bDisableChromaticAberration),    GetBoolStr(bDisableChromaticAberration),    bDisableChromaticAberration);
    LOG(L"%s: %s (%i)\n", wstr(bDisableDoF),                    GetBoolStr(bDisableDoF),                    bDisableDoF);
    LOG(L"%s: %s (%i)\n", wstr(bDisableVignette),               GetBoolStr(bDisableVignette),               bDisableVignette);
}

void DisableTAA(wchar_t* exeName)
{
    const unsigned char patchTAA[] = { 0xEB, 0x04 };

    if (!StrCmpW(exeName, u4_exe))
    {
        WritePatchPattern(L"75 04 B0 01 EB 02 32 C0 41 88 86 93 14 01 00", patchTAA, sizeof(patchTAA), L"Disable TAA", 0);
        const unsigned char patchJitter[] = { 0x40, 0x32, 0xF6, 0x90 };
        WritePatchPattern(L"C5 C0 57 FF 40 0F 95 C6", patchJitter, sizeof(patchJitter), L"Disable Camera Jitter", 0);
    }
    else if (!StrCmpW(exeName, u4_l_exe))
    {
        WritePatchPattern(L"75 04 B0 01 EB 02 32 C0 41 88 86 93 14 01 00", patchTAA, sizeof(patchTAA), L"Disable TAA", 0);
        const unsigned char patchJitter[] = { 0x40, 0x32, 0xFF, 0x90 };
        WritePatchPattern(L"C5 C8 57 F6 40 0F 95 C7", patchJitter, sizeof(patchJitter), L"Disable Camera Jitter", 0);
    }
    else if (!StrCmpW(exeName, tll_exe))
    {
        WritePatchPattern(L"75 04 B0 01 EB 02 32 C0 41 88 86 F3 14 01 00", patchTAA, sizeof(patchTAA), L"Disable TAA", 0);
        const unsigned char patchJitter[] = { 0x45, 0x32, 0xF6, 0x90 };
        WritePatchPattern(L"C5 C8 57 F6 41 0F 95 C6", patchJitter, sizeof(patchJitter), L"Disable Camera Jitter", 0);
    }
    else if (!StrCmpW(exeName, tll_l_exe))
    {
        WritePatchPattern(L"75 04 B0 01 EB 02 32 C0 41 88 86 F3 14 01 00", patchTAA, sizeof(patchTAA), L"Disable TAA", 0);
        const unsigned char patchJitter[] = { 0x40, 0x32, 0xED, 0x90 };
        WritePatchPattern(L"C5 C8 57 F6 40 0F 95 C5", patchJitter, sizeof(patchJitter), L"Disable Camera Jitter", 0);
    }

}
//tested, OK

void DisableDoF(wchar_t* exeName)
{
    const unsigned char patchDoF[] = { 0x00, 0xB8, 0x00, 0x00, 0x80, 0x7F, 0x90, 0x90 };

    if (!StrCmpW(exeName, u4_exe) || !StrCmpW(exeName, u4_l_exe))
    {
        WritePatchPattern(L"00 41 8B 87 20 0C 00 00", patchDoF, sizeof(patchDoF), L"Disable DoF", 0);
    }
    else if (!StrCmpW(exeName, tll_exe) || !StrCmpW(exeName, tll_l_exe))
    {
        WritePatchPattern(L"00 41 8B 87 B0 0C 00 00", patchDoF, sizeof(patchDoF), L"Disable DoF", 0);
    }
}

void DisableBarrelDistortion(wchar_t* exeName)
{
    const unsigned char patchBarrelDistortion[] = { 0x00, 0x33, 0xC0, 0x90, 0x90, 0x90, 0x90, 0x90 };

    if (!StrCmpW(exeName, u4_exe) || !StrCmpW(exeName, u4_l_exe))
    {
        WritePatchPattern(L"00 41 8B 86 48 0D 00 00", patchBarrelDistortion, sizeof(patchBarrelDistortion), L"Disable Barrel Distortion", 0);
    }
    else if (!StrCmpW(exeName, tll_exe) || !StrCmpW(exeName, tll_l_exe))
    {
        WritePatchPattern(L"00 41 8B 87 D4 0D 00 00", patchBarrelDistortion, sizeof(patchBarrelDistortion), L"Disable Barrel Distortion", 0);
    }
}

void DisableScreenZoom(wchar_t* exeName)
{
    const unsigned char patchScreenZoom[] = { 0x00, 0xB8, 0x00, 0x00, 0x80, 0x3F, 0x90, 0x90 };

    if (!StrCmpW(exeName, u4_exe) || !StrCmpW(exeName, u4_l_exe))
    {
        WritePatchPattern(L"00 41 8B 86 4C 0D 00 00", patchScreenZoom, sizeof(patchScreenZoom), L"Disable Screen Zoom", 0);
    }
    else if (!StrCmpW(exeName, tll_exe) || !StrCmpW(exeName, tll_l_exe))
    {
        WritePatchPattern(L"00 41 8B 87 D8 0D 00 00", patchScreenZoom, sizeof(patchScreenZoom), L"Disable Screen Zoom", 0);
    }
}

void DisableChromaticAberration(wchar_t* exeName)
{
    const unsigned char patchChromaticAberration[] = { 0x00, 0x33, 0xC0, 0x90, 0x90, 0x90, 0x90, 0x90 };

    if (!StrCmpW(exeName, u4_exe) || !StrCmpW(exeName, u4_l_exe))
    {
        WritePatchPattern(L"00 41 8B 86 50 0D 00 00", patchChromaticAberration, sizeof(patchChromaticAberration), L"Disable Chromatic Aberration", 0);
    }
    else if (!StrCmpW(exeName, tll_exe) || !StrCmpW(exeName, tll_l_exe))
    {
        WritePatchPattern(L"00 41 8B 87 DC 0D 00 00", patchChromaticAberration, sizeof(patchChromaticAberration), L"Disable Chromatic Aberration", 0);
    }
}

void DisableSharpening(wchar_t* exeName)
{
    const unsigned char patchSharpening[] = { 0x32, 0xC0, 0xEB, 0x04, 0x32, 0xDB, 0x32, 0xC0 };

    if (!StrCmpW(exeName, u4_exe) || !StrCmpW(exeName, u4_l_exe))
    {
        WritePatchPattern(L"B0 01 EB 04 32 DB 32 C0", patchSharpening, sizeof(patchSharpening), L"Disable Sharpening", 0);
    }
    else if (!StrCmpW(exeName, tll_exe) || !StrCmpW(exeName, tll_l_exe))
    {
        WritePatchPattern(L"B0 01 EB 04 32 DB 32 C0", patchSharpening, sizeof(patchSharpening), L"Disable Sharpening", 0);
    }
}

void DisableVignette(wchar_t* exeName)
{
    if (!StrCmpW(exeName, u4_exe))
    {
        const unsigned char patchVignette[] = { 0x0F, 0x57, 0xC0, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
        WritePatchPattern(L"C4 C1 7A 10 86 18 0D 00 00", patchVignette, sizeof(patchVignette), L"Disable Vignette", 0);
    }
    else if (!StrCmpW(exeName, u4_l_exe))
    {
        const unsigned char patchVignette[] = { 0x0F, 0x57, 0xC9, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
        WritePatchPattern(L"C4 C1 7A 10 8E 18 0D 00 00", patchVignette, sizeof(patchVignette), L"Disable Vignette", 0);
    }
    else if (!StrCmpW(exeName, tll_exe) || !StrCmpW(exeName, tll_l_exe))
    {
        const unsigned char patchVignette[] = { 0x45, 0x0F, 0x57, 0xC9, 0x90, 0x90, 0x90, 0x90, 0x90 };
        WritePatchPattern(L"C4 41 7A 10 8F A4 0D 00 00", patchVignette, sizeof(patchVignette), L"Disable Vignette", 0);
    }
}

void EnbaleDebugMenu()
{
    CodeCave::DMenu_Addr = ReadLEA32(Patterns::DMenu_Ptr, wstr(Patterns::DMenu_Ptr), 0, 3, 7);
    CodeCave::DebugDraw_UpdateLoopAddr = FindAndPrintPatternW(Patterns::DebugDraw_UpdateLoop, wstr(Pattern::DebugDraw_UpdateLoop));
    CodeCave::DMenu_Update = FindAndPrintPatternW(Patterns::DMenu_Update, wstr(Pattern::DMenu_Update));
    if (CodeCave::DMenu_Addr && CodeCave::DebugDraw_UpdateLoop && CodeCave::DMenu_Update)
    {
        WritePatchPattern_Hook(Patterns::DebugDraw_UpdateLoop, 22, wstr(Patterns::DebugDraw_UpdateLoop), 0, (void*)CodeCave::DebugDraw_UpdateLoop, nullptr);
        CodeCave::DebugDraw_UpdateLoopReturnAddr = CodeCave::DebugDraw_UpdateLoopAddr + 22;
    }
}

void SkipIntroLogos()
{
    uintptr_t skiplogos_clargAddr = FindAndPrintPatternW(Patterns::skiplogos_clarg, wstr(Patterns::skiplogos_clarg));
    if (skiplogos_clargAddr)
    {
        const uint8_t long_jmp[] = { 0x48, 0xe9 };
        Memory::PatchBytes(skiplogos_clargAddr + 9, long_jmp, sizeof(long_jmp));
    }
}

void KillPrintf()
{
    const uint8_t ret_0[] = { 0x31, 0xc0, 0xc3 };
    WritePatchPattern(Patterns::GamePrintf, ret_0, sizeof(ret_0), wstr(Patterns::GamePrintf), 0);
}

DWORD __stdcall Main()
{
    baseModule = GetModuleHandle(NULL);
    bLoggingEnabled = false;
    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, _PROJECT_LOG_PATH);
    LoggingInit(_PROJECT_NAME, LogPath);
    LOG(L"GetModuleHandle: 0x%p\n", baseModule);
    ReadConfig();

    wchar_t* exeName = GetModuleName(exePath);
    KillPrintf();
    if (bDisableTAA)
        DisableTAA(exeName);
    if (bDisableSharpening)
        DisableSharpening(exeName);
    if (bDisableBarrelDistortion)
        DisableBarrelDistortion(exeName);
    if (bDisableScreenZoom)
        DisableScreenZoom(exeName);
    if (bDisableChromaticAberration)
        DisableChromaticAberration(exeName);
    if (bDisableDoF)
        DisableDoF(exeName);
    if (bDisableVignette)
        DisableVignette(exeName);
    if (bDebugMenu)
        EnbaleDebugMenu();
    if (bSkipIntroLogos)
        SkipIntroLogos();
    LOG(L"Shutting down " wstr(fp_log) " file handle.\n");
    fclose(fp_log);
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule,
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
