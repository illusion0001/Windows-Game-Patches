#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "patterns.hpp"
#include "code_caves.hpp"

HMODULE baseModule = GetModuleHandle(NULL);
#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "T1X.DebugFeatures"
#define _PROJECT_NAME L"" PROJECT_NAME
#define PROJECT_LOG_PATH PROJECT_NAME ".log"
#define _PROJECT_LOG_PATH L"" PROJECT_LOG_PATH
#define BUILD_TIME PROJECT_NAME " Built: " __DATE__ " @ " __TIME__

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bDebugMenu;
bool bShowDebugConsole;
float fDebugMenuSize;
bool bExtendedDebugMenu;

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LOG(L"" BUILD_TIME "\n");
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
                                    wstr(bDebugMenu)" = true\n"
                                    wstr(bShowDebugConsole)" = false\n"
                                    wstr(fDebugMenuSize)" = 0.6\n"
                                    "; Extended Debug Menu by infogram\n"
                                    wstr(bExtendedDebugMenu)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDebugMenu = true;
        bShowDebugConsole = false;
        bExtendedDebugMenu = true;
        fDebugMenuSize = 0.6;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDebugMenu), bDebugMenu);
        inipp::get_value(ini.sections[L"Settings"], wstr(bShowDebugConsole), bShowDebugConsole);
        inipp::get_value(ini.sections[L"Settings"], wstr(bExtendedDebugMenu), bExtendedDebugMenu);
        inipp::get_value(ini.sections[L"Settings"], wstr(fDebugMenuSize), fDebugMenuSize);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDebugMenu), GetBoolStr(bDebugMenu) , bDebugMenu);
    LOG(L"%s: %s (%i)\n", wstr(bShowDebugConsole), GetBoolStr(bShowDebugConsole), bShowDebugConsole);
    LOG(L"%s: (%f)\n",    wstr(fDebugMenuSize), fDebugMenuSize);
    LOG(L"%s: %s (%i)\n", wstr(bExtendedDebugMenu), GetBoolStr(bExtendedDebugMenu), bExtendedDebugMenu);
}

FILE* fGame;

void ApplyDebugPatches(void)
{
    if (bDebugMenu)
    {
        if (fDebugMenuSize > 1)
            fDebugMenuSize = 1.0;
        else if (fDebugMenuSize < 0.4)
            fDebugMenuSize = 0.4;
        // This is considered [strict aliasing violation](https://stackoverflow.com/a/45229284)
        // But it does work
        WritePatchPattern_Int(4, Patterns::DevMenu_MenuSize, (void*)*(uint32_t*)&fDebugMenuSize, wstr(Patterns::DevMenu_MenuSize), 3);
        const unsigned char mov_ecx_0[] = { 0xb9, 0x00, 0x00, 0x00, 0x00, 0x90 };
        const unsigned char nop1x[] = { 0x90 };
        WritePatchPattern(Patterns::m_onDisc_DevMenu, mov_ecx_0, sizeof(mov_ecx_0), wstr(Patterns::m_onDisc_DevMenu), 0);
        WritePatchPattern(Patterns::Assert_LevelDef_LevelManifst, nop1x, sizeof(nop1x), wstr(Patterns::Assert_LevelDef_LevelManifst), 30);
        WritePatchPattern_Hook(Patterns::GivePlayerWeapon_Main, 29, wstr(Patterns::GivePlayerWeapon_Main), 0, &GivePlayerWeapon_MainCC, &GivePlayerWeapon_MainReturn);
        WritePatchPattern_Hook(Patterns::GivePlayerWeapon_SubSection, 21, wstr(Patterns::GivePlayerWeapon_SubSection), 0, &GivePlayerWeapon_SubCC, &GivePlayerWeapon_SubReturn);
        WritePatchPattern_Hook(Patterns::GivePlayerWeapon_Entry, 21, wstr(Patterns::GivePlayerWeapon_Entry), 0, &GivePlayerWeapon_EntryCC, &GivePlayerWeapon_EntryReturn);
        AllocMemoryforStructureAddr = FindAndPrintPatternW(Patterns::AllocMemoryforStructure, wstr(Patterns::AllocMemoryforStructure));
        CreateDevMenuStructureAddr = FindAndPrintPatternW(Patterns::CreateDevMenuStructure, wstr(Patterns::CreateDevMenuStructure));
        AllocDevMenuMemoryforStructureAddr = FindAndPrintPatternW(Patterns::AllocDevMenuMemoryforStructure, wstr(Patterns::AllocDevMenuMemoryforStructure));
        AllocDevMenu1Addr = FindAndPrintPatternW(Patterns::AllocDevMenu1, wstr(Patterns::AllocDevMenu1));
        DevMenuCreateHeaderAddr = FindAndPrintPatternW(Patterns::DevMenuCreateHeader, wstr(Patterns::DevMenuCreateHeader));
        DevMenuCreateEntryAddr = FindAndPrintPatternW(Patterns::DevMenuCreateEntry, wstr(Patterns::DevMenuCreateEntry));
        DevMenuAddBoolAddr = FindAndPrintPatternW(Patterns::DevMenuAddBool, wstr(Patterns::DevMenuAddBool));
        DevMenuAddFuncButtonAddr = FindAndPrintPatternW(Patterns::DevMenuAddFuncButton, wstr(Patterns::DevMenuAddFuncButton));
        DevMenuAddIntSliderAddr = FindAndPrintPatternW(Patterns::DevMenuAddIntSlider, wstr(Patterns::DevMenuAddIntSlider));
        uintptr_t MeleeMenuResult = FindAndPrintPatternW(Patterns::MeleeMenuHook, wstr(Patterns::MeleeMenuHook));
        if (
            AllocMemoryforStructureAddr &&
            CreateDevMenuStructureAddr &&
            AllocDevMenuMemoryforStructureAddr &&
            AllocDevMenu1Addr &&
            DevMenuCreateHeaderAddr &&
            DevMenuCreateEntryAddr &&
            DevMenuAddBoolAddr &&
            DevMenuAddFuncButtonAddr &&
            DevMenuAddIntSliderAddr &&
            MeleeMenuResult
            )
        {
            _snprintf_s(BuildVer, sizeof(BuildVer), BUILD_TIME);
            WritePatchPattern_Hook(Patterns::MeleeMenuHook, 14, wstr(Patterns::MeleeMenuHook), 0, (void*)MakeMeleeMenu, nullptr);
        }
        ScriptLookupAddr = uintptr_t(Memory::PatternScanW(baseModule, Patterns::ScriptManager_LookupClass));
        uintptr_t EvalScriptWarns = uintptr_t(Memory::PatternScanW(baseModule, Patterns::GameWarnScriptPrint2));
        if (EvalScriptWarns)
        {
            // First param is buffer
            // Sprintf eqiv
            const unsigned char jmp_rcx[] = { 0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe1 };
            uintptr_t PrintAddr = WritePatchPattern(Patterns::GameWarnScriptPrint, jmp_rcx, sizeof(jmp_rcx), wstr(Patterns::GameWarnScriptPrint), 0);
            unsigned char bytes_8[8] = { 0 };
            uintptr_t hook_ptr = uintptr_t((void*)ScriptPrintWarn_CC);
            memcpy((void*)bytes_8, &hook_ptr, sizeof(bytes_8));
            WritePatchAddress(PrintAddr, bytes_8, sizeof(bytes_8), wstr(ScriptPrintWarn_CC), 2);
        }
        else
        {
            LOG(wstr(EvalScriptWarns) L": 0x%016llx\n", EvalScriptWarns);
        }
    }
    if (bShowDebugConsole)
    {
        AllocConsole();
        freopen_s(&fGame, "CONOUT$", "w", stdout);
        freopen_s(&fGame, "CONOUT$", "w", stderr);
        freopen_s(&fGame, "CONOUT$", "w", stdin);
        printf_s(BUILD_TIME "\n");
        fprintf_s(fGame, BUILD_TIME "\n");
    }
    if (bExtendedDebugMenu)
    {
        const unsigned char nop5x[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
        const unsigned char ret_1_al[] = { 0xb0, 0x01, 0xc3 };
        const unsigned char nop1x[] = { 0x90 };
        const unsigned char ret_0[] = { 0x31, 0xc0, 0xc3 };
        const unsigned char heap_type[] = { MEM_TYPE };
        uint64_t menu_mem_size = 0x840000;
        uint64_t script_mem_size = 0x800000;
        uint64_t cpu_mem_size = 0x40e00000;
        WritePatchPattern_Hook(Patterns::Memory_PushAllocatorJMPAddr, 15, wstr(Patterns::Memory_PushAllocatorJMPAddr), 0, &Memory_PushAllocator_CC, &Memory_PushAllocatorReturnAddr);
        WritePatchPattern(Patterns::Memory_isDebugMemoryAval, ret_1_al, sizeof(ret_1_al), wstr(Patterns::Memory_isDebugMemoryAval), 0);
        WritePatchPattern(Patterns::DebugDrawStaticContext, nop5x, sizeof(nop5x), wstr(Patterns::DebugDrawStaticContext), 0);
        WritePatchPattern(Patterns::MaterialDebug_Heap, heap_type, sizeof(heap_type), wstr(Patterns::MaterialDebug_Heap), 4);
        WritePatchPattern(Patterns::ParticlesMenu, ret_0, sizeof(ret_0), wstr(Patterns::ParticlesMenu), 0);
        WritePatchPattern(Patterns::UpdateSelectRegionByNameMenu_Heap, heap_type, sizeof(heap_type), wstr(Patterns::UpdateSelectRegionByNameMenu_Heap), 9);
        WritePatchPattern(Patterns::UpdateSelectSpawner, heap_type, sizeof(heap_type), wstr(Patterns::UpdateSelectSpawner), 4);
        WritePatchPattern_Int(sizeof(menu_mem_size), Patterns::MenuHeap_UsableMemorySize, (void*)menu_mem_size, wstr(Patterns::MenuHeap_UsableMemorySize), 8);
        WritePatchPattern_Int(sizeof(script_mem_size), Patterns::ScriptHeap_UsableMemorySize, (void*)script_mem_size, wstr(Patterns::ScriptHeap_UsableMemorySize), 8);
        WritePatchPattern_Int(sizeof(cpu_mem_size), Patterns::CPUHeap_UsableMemorySize, (void*)cpu_mem_size, wstr(Patterns::CPUHeap_UsableMemorySize), 8);
        WritePatchPattern(Patterns::Memory_ValidateContext, nop1x, sizeof(nop1x), wstr(Patterns::Memory_ValidateContext), 0);
        WritePatchPattern(Patterns::Assert_UpdateSelectRegionByNameMenu, nop1x, sizeof(nop1x), wstr(Patterns::Assert_UpdateSelectRegionByNameMenu), 0);
        WritePatchPattern(Patterns::Assert_UpdateSelectIgcByNameMenu, nop1x, sizeof(nop1x), wstr(Patterns::Assert_UpdateSelectIgcByNameMenu), 0);
        WritePatchPattern(Patterns::Assert_UpdateSelectSpawnerByNameMenu, nop1x, sizeof(nop1x), wstr(Patterns::Assert_UpdateSelectSpawnerByNameMenu), 0);
    }
}

void __stdcall Main()
{
    bLoggingEnabled = false;
    bDebugMenu = false;
    bShowDebugConsole = false;
    bExtendedDebugMenu = false;
    fDebugMenuSize = 0.6;
    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, _PROJECT_LOG_PATH);
    LoggingInit(_PROJECT_NAME, LogPath);
    ReadConfig();
    ApplyDebugPatches();
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
