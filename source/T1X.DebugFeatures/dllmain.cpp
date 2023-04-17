#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "patterns.hpp"
#include "code_caves.hpp"

HMODULE baseModule = GetModuleHandle(NULL);
#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"T1X.DebugFeatures"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

// INI Variables
bool bDebugMenu;
bool bShowDebugConsole;
float fDebugMenuSize;
bool bExtendedDebugMenu;

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
        Game_SnprintfAddr = uintptr_t(Memory::PatternScanW(baseModule, Patterns::GameSnprintf));
        LOG(wstr(Patterns::GameSnprintf) L": 0x%016llx\n", Game_SnprintfAddr);
        WritePatchPattern(Patterns::m_onDisc_DevMenu, mov_ecx_0, sizeof(mov_ecx_0), wstr(Patterns::m_onDisc_DevMenu), 0);
        WritePatchPattern(Patterns::Assert_LevelDef_LevelManifst, nop1x, sizeof(nop1x), wstr(Patterns::Assert_LevelDef_LevelManifst), 30);
        WritePatchPattern_Hook(Patterns::GivePlayerWeapon_Main, 29, wstr(Patterns::GivePlayerWeapon_Main), 0, &GivePlayerWeapon_MainCC, &GivePlayerWeapon_MainReturn);
        WritePatchPattern_Hook(Patterns::GivePlayerWeapon_SubSection, 21, wstr(Patterns::GivePlayerWeapon_SubSection), 0, &GivePlayerWeapon_SubCC, &GivePlayerWeapon_SubReturn);
        WritePatchPattern_Hook(Patterns::GivePlayerWeapon_Entry, 21, wstr(Patterns::GivePlayerWeapon_Entry), 0, &GivePlayerWeapon_EntryCC, &GivePlayerWeapon_EntryReturn);
    }
    if (bShowDebugConsole)
    {
        const unsigned char mov_cl_1_nop[] = { 0xb1, 0x01, 0x90 };
        WritePatchPattern(Patterns::ConsoleOutput, mov_cl_1_nop, sizeof(mov_cl_1_nop), wstr(Patterns::ConsoleOutput), 0);
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
    LoggingInit(_PROJECT_NAME, _PROJECT_LOG_PATH);
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
