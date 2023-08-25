#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "patterns.hpp"
#include "code_caves.hpp"
#include "dmenu.hpp"
#include "git_ver.h"

HMODULE baseModule = GetModuleHandle(NULL);
#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "T1X.DebugFeatures"
#define PROJECT_LOG_PATH PROJECT_NAME ".log"
#define BUILD_TIME PROJECT_NAME " Built: " __DATE__ " @ " __TIME__

wchar_t exePath[_MAX_PATH]{};

// INI Variables
bool bDebugMenu{};
bool bShowDebugConsole{};
float fDebugMenuSize{};
bool bExtendedDebugMenu{};
bool bTestMode{};

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LOG(L"" GIT_COMMIT "\n");
    LOG(L"" GIT_VER "\n");
    LOG(L"" GIT_NUM "\n");
    LOG(L"" BUILD_TIME "\n");
    LOG(L"Game Name: %s\n", Memory::GetVersionProductName().c_str());
    LOG(L"Game Path: %s\n", exePath);

    // Initialize config
    std::wstring config_path = L"" PROJECT_NAME ".ini";
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
        inipp::get_value(ini.sections[L"Settings"], wstr(bTestMode), bTestMode);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDebugMenu), GetBoolStr(bDebugMenu) , bDebugMenu);
    LOG(L"%s: %s (%i)\n", wstr(bShowDebugConsole), GetBoolStr(bShowDebugConsole), bShowDebugConsole);
    LOG(L"%s: (%f)\n",    wstr(fDebugMenuSize), fDebugMenuSize);
    LOG(L"%s: %s (%i)\n", wstr(bExtendedDebugMenu), GetBoolStr(bExtendedDebugMenu), bExtendedDebugMenu);
}

FILE* fGame;

constexpr const uint32_t GameVer1050 = 3591618;
constexpr const uint32_t GameVer1100 = 3613846;

void ApplyDebugPatches(void)
{
    const char* game_ver_text = "BUILD_NUMBER=";
    constexpr const size_t game_ver_len = strlen(game_ver_text);
    uintptr_t version_number = (uintptr_t)Memory::char_Scan(baseModule, game_ver_text, game_ver_len - 1);
    uint32_t game_ver_int = 0;
    if (version_number)
    {
        game_ver_int = strtol((const char*)version_number + game_ver_len, nullptr, 10);
    }
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
        if (game_ver_int > 1 && game_ver_int < GameVer1050)
        {
            WritePatchPattern_Hook(Patterns::GivePlayerWeapon_Main, 29, wstr(Patterns::GivePlayerWeapon_Main), 0, (void*)GivePlayerWeapon_MainCC, &GivePlayerWeapon_MainReturn);
            WritePatchPattern_Hook(Patterns::GivePlayerWeapon_SubSection, 21, wstr(Patterns::GivePlayerWeapon_SubSection), 0, (void*)GivePlayerWeapon_SubCC, &GivePlayerWeapon_SubReturn);
            WritePatchPattern_Hook(Patterns::GivePlayerWeapon_Entry, 21, wstr(Patterns::GivePlayerWeapon_Entry), 0, (void*)GivePlayerWeapon_EntryCC, &GivePlayerWeapon_EntryReturn);
            GameVeris1050 = false;
        }
        else if (game_ver_int > 1 && game_ver_int >= GameVer1050)
        {
            uintptr_t MainAddr = FindAndPrintPatternW(Patterns::GivePlayerWeapon_Main1050, wstr(Patterns::GivePlayerWeapon_Main1050));
            uintptr_t SubAddr = FindAndPrintPatternW(Patterns::GivePlayerWeapon_SubSection1050, wstr(Patterns::GivePlayerWeapon_SubSection1050));
            uintptr_t EntryAddr = FindAndPrintPatternW(Patterns::GivePlayerWeapon_Entry1050, wstr(Patterns::GivePlayerWeapon_Entry1050));
            uintptr_t EntryHeaderAddr = FindAndPrintPatternW(Patterns::GivePlayerWeapon_EntryHeader1050, wstr(Patterns::GivePlayerWeapon_EntryHeader1050));
            uintptr_t JumpPattern = FindAndPrintPatternW(Patterns::Int3_14bytes, wstr(Patterns::Int3_14bytes));
            Make32to64Hook((void*)MainAddr, (void*)JumpPattern, (void*)GivePlayerWeapon_MainCC, 16, wstr(Patterns::GivePlayerWeapon_Main1050), wstr(Patterns::Int3_14bytes), wstr(GivePlayerWeapon_MainCC));
            JumpPattern = FindAndPrintPatternW(Patterns::Int3_14bytes, wstr(Patterns::Int3_14bytes));
            Make32to64Hook((void*)SubAddr, (void*)JumpPattern, (void*)GivePlayerWeapon_SubCC, 16, wstr(Patterns::GivePlayerWeapon_SubSection1050), wstr(Patterns::Int3_14bytes), wstr(GivePlayerWeapon_SubCC));
            JumpPattern = FindAndPrintPatternW(Patterns::Int3_14bytes, wstr(Patterns::Int3_14bytes));
            Make32to64Hook((void*)EntryAddr, (void*)JumpPattern, (void*)GivePlayerWeapon_EntryCC, 7, wstr(Patterns::GivePlayerWeapon_Entry1050), wstr(Patterns::Int3_14bytes), wstr(GivePlayerWeapon_EntryCC));
            JumpPattern = FindAndPrintPatternW(Patterns::Int3_14bytes, wstr(Patterns::Int3_14bytes));
            Make32to64Hook((void*)EntryHeaderAddr, (void*)JumpPattern, (void*)GivePlayerWeapon_EntryHeaderCC, 7, wstr(Patterns::GivePlayerWeapon_EntryHeader1050), wstr(Patterns::Int3_14bytes), wstr(GivePlayerWeapon_EntryHeaderCC));
            GivePlayerWeapon_MainReturn = MainAddr + 16;
            GivePlayerWeapon_SubReturn = SubAddr + 16;
            GivePlayerWeapon_EntryReturn = EntryAddr + 7;
            GivePlayerWeapon_EntryHeaderReturn = EntryHeaderAddr + 7;
            GameVeris1050 = true;
        }
        CreateDevMenuStructure_Caller = (CreateDevMenuStructure_Caller_ptr)FindAndPrintPatternW(Patterns::CreateDevMenuStructure, wstr(Patterns::CreateDevMenuStructure));
        AllocDevMenuMemoryforStructure_Caller = (AllocDevMenuMemoryforStructure_Caller_ptr)FindAndPrintPatternW(Patterns::AllocDevMenuMemoryforStructure, wstr(Patterns::AllocDevMenuMemoryforStructure));
        DevMenuCreateHeader_Caller = (DevMenuCreateHeader_Caller_ptr)FindAndPrintPatternW(Patterns::DevMenuCreateHeader, wstr(Patterns::DevMenuCreateHeader));
        DevMenuCreateEntry_Caller = (DevMenuCreateEntry_Caller_ptr)FindAndPrintPatternW(Patterns::DevMenuCreateEntry, wstr(Patterns::DevMenuCreateEntry));
        DevMenuAddBool_Caller = (DevMenuAddBool_Caller_ptr)FindAndPrintPatternW(Patterns::DevMenuAddBool, wstr(Patterns::DevMenuAddBool));
        DevMenuAddFuncButton_Caller = (DevMenuAddFuncButton_Caller_ptr)FindAndPrintPatternW(Patterns::DevMenuAddFuncButton, wstr(Patterns::DevMenuAddFuncButton));
        DevMenuAddIntSlider_Caller = (DevMenuAddIntSlider_Caller_ptr)FindAndPrintPatternW(Patterns::DevMenuAddIntSlider, wstr(Patterns::DevMenuAddIntSlider));
        uintptr_t MeleeMenuResult = FindAndPrintPatternW(Patterns::MeleeMenuHook, wstr(Patterns::MeleeMenuHook));
        EntitySpawner_Caller = (EntitySpawner_Caller_ptr)FindAndPrintPatternW(Patterns::EntitySpawner, wstr(Patterns::EntitySpawner), -0x34);
        LoadLevelByName_Caller = (LoadLevelByName_Caller_ptr)FindAndPrintPatternW(Patterns::LoadLevelByName, wstr(Patterns::LoadLevelByName));
        LoadActorByName_Caller = (LoadActorByName_Caller_ptr)FindAndPrintPatternW(Patterns::LoadActorByName, wstr(Patterns::LoadActorByName));
        uintptr_t PlayerPtrAddrJMP = FindAndPrintPatternW(Patterns::PlayerPtr, wstr(Patterns::PlayerPtr));
        ReadCurrentLookIDAddr = FindAndPrintPatternW(Patterns::ReadCurrentLookID, wstr(Patterns::ReadCurrentLookID));
        ActiveTaskDisplayAddr = FindAndPrintPatternW(Patterns::ActiveTaskDisplay, wstr(Patterns::ActiveTaskDisplay));
        uintptr_t DebugPrintAddr = FindAndPrintPatternW(Patterns::CharPrintText, wstr(Patterns::CharPrintText));
        TextPrintV = (TextPrintV_ptr)FindAndPrintPatternW(Patterns::TextPrintV, wstr(Patterns::TextPrintV));
        InitProfileMenuAddr = FindAndPrintPatternW(Patterns::InitProfileMenu, wstr(Patterns::InitProfileMenu));
        DevMenuCreateSeparationLine_Caller = (DevMenuCreateSeparationLine_Caller_ptr)FindAndPrintPatternW(Patterns::DevMenuCreateSeparationLine, wstr(Patterns::DevMenuCreateSeparationLine));
        DevMenuCreateCyanSubText_Caller = (DevMenuCreateCyanSubText_Caller_ptr)FindAndPrintPatternW(Patterns::DevMenuCreateCyanSubText, wstr(Patterns::DevMenuCreateCyanSubText));
        RestartCheckpointInternal_Caller = (RestartCheckpointInternal_Caller_ptr)FindAndPrintPatternW(Patterns::RestartCheckpointInternal, wstr(Patterns::RestartCheckpointInternal));
        DevMenuCreateSelection_Caller = (DevMenuCreateSelection_Caller_ptr)FindAndPrintPatternW(Patterns::DevMenuCreateSelection, wstr(Patterns::DevMenuCreateSelection));
        DevMenuSelectionCallback_Caller = (DevMenuSelectionCallback_Caller_ptr)FindAndPrintPatternW(Patterns::DevMenuSelectionCallback, wstr(Patterns::DevMenuSelectionCallback));
        ScriptManager_LookupClass = (ScriptManager_LookupClass_ptr)FindAndPrintPatternW(Patterns::ScriptManager_LookupClass, wstr(Patterns::ScriptManager_LookupClass));
        uintptr_t JumpPattern = 0;
        if (
            CreateDevMenuStructure_Caller &&
            AllocDevMenuMemoryforStructure_Caller &&
            DevMenuCreateHeader_Caller &&
            DevMenuCreateEntry_Caller &&
            DevMenuAddBool_Caller &&
            DevMenuAddFuncButton_Caller &&
            DevMenuAddIntSlider_Caller &&
            MeleeMenuResult &&
            EntitySpawner_Caller &&
            LoadLevelByName_Caller &&
            LoadActorByName_Caller &&
            PlayerPtrAddrJMP &&
            ReadCurrentLookIDAddr &&
            ActiveTaskDisplayAddr &&
            DebugPrintAddr &&
            TextPrintV &&
            InitProfileMenuAddr &&
            DevMenuCreateSeparationLine_Caller &&
            DevMenuCreateCyanSubText_Caller &&
            RestartCheckpointInternal_Caller &&
            ScriptManager_LookupClass
            )
        {
            strncpy_s(BuildVer, sizeof(BuildVer), BUILD_TIME, sizeof(BuildVer));
            WritePatchPattern_Hook(Patterns::MeleeMenuHook, 14, wstr(Patterns::MeleeMenuHook), 0, (void*)MakeMeleeMenu, nullptr);
            PlayerPtrAddrJMP = PlayerPtrAddrJMP + 0x48;
            JumpPattern = FindAndPrintPatternW(Patterns::Int3_14bytes, wstr(Patterns::Int3_14bytes));
            Make32to64Hook((void*)PlayerPtrAddrJMP, (void*)JumpPattern, (void*)GetPlayerPtrAddr_CC, 6, wstr(PlayerPtrAddrJMP), wstr(Patterns::Int3_14bytes), wstr(GetPlayerPtrAddr_CC));
            Memory::DetourFunction64((void*)ReadCurrentLookIDAddr, (void*)ReadLookID_Hook, 14);
            WritePatchPattern_Hook(Patterns::ActiveTaskDisplay, 24, wstr(Patterns::ActiveTaskDisplay), 0, (void*)ActiveTaskDisplay_CC, &ActiveTaskDisplayReturnAddr);
            DebugPrintAddr = DebugPrintAddr + 24;
            uint32_t OffsetToOriginalPrint = *(uint32_t*)(DebugPrintAddr + 1);
            DebugPrint_OriginalAddr = DebugPrintAddr + OffsetToOriginalPrint + 5;
            JumpPattern = FindAndPrintPatternW(Patterns::Int3_14bytes, wstr(Patterns::Int3_14bytes));
            Make32to64Hook((void*)DebugPrintAddr, (void*)JumpPattern, (void*)DebugPrint_CC, 5, wstr(DebugPrintAddr), wstr(Patterns::Int3_14bytes), wstr(DebugPrint_CC));
            DebugPrint_ReturnAddr = DebugPrintAddr + 5;
        }
    }
    if (bDebugMenu && bExtendedDebugMenu)
    {
        const unsigned char nop5x[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
        const unsigned char ret_1_al[] = { 0xb0, 0x01, 0xc3 };
        const unsigned char nop1x[] = { 0x90 };
        const unsigned char ret_0[] = { 0x31, 0xc0, 0xc3 };
        constexpr uint64_t size_mb = 1048576;
        constexpr uint64_t menu_mem_size = (uint64_t)8 * size_mb;
        constexpr uint64_t script_mem_size = (uint64_t)8 * size_mb;
        uint64_t cpu_mem_size = 0;
        if (!GameVeris1050)
        {
            cpu_mem_size = (uint64_t)1040 * size_mb;
        }
        else
        {
            cpu_mem_size = (uint64_t)1152 * size_mb;
        }
        int32_t OffsetToisDebugMemoryAval = 0;
        uintptr_t ParticlesMenu = FindAndPrintPatternW(Patterns::ParticlesMenu, wstr(Patterns::ParticlesMenu));
        ParticlesMenu = ParticlesMenu + 12;
        OffsetToisDebugMemoryAval = *(uint32_t*)(ParticlesMenu);
        WritePatchAddress(ParticlesMenu + OffsetToisDebugMemoryAval + 4, ret_1_al, sizeof(ret_1_al), wstr(Patterns::Memory_isDebugMemoryAval), 0);
        WritePatchPattern_Hook(Patterns::Memory_PushAllocator, 15, wstr(Patterns::Memory_PushAllocator), 0, (void*)Memory_PushAllocator_CC, &Memory_PushAllocatorReturnAddr);
        if(game_ver_int > 1 && game_ver_int < GameVer1100)
        {
            WritePatchPattern_Hook(Patterns::Memory_NewHandler, 16, wstr(Patterns::Memory_NewHandler), 0, (void*)Memory_NewHandler_CC, &Memory_NewHandlerReturnAddr);
            GameVeris1100 = false;
        }
        else if (game_ver_int > 1 && game_ver_int >= GameVer1100)
        {
            WritePatchPattern_Hook(Patterns::Memory_NewHandler1100, 17, wstr(Patterns::Memory_NewHandler1100), 0, (void*)Memory_NewHandler_CC, &Memory_NewHandlerReturnAddr);
            GameVeris1100 = true;
        }
        WritePatchPattern(Patterns::DebugDrawStaticContext, nop5x, sizeof(nop5x), wstr(Patterns::DebugDrawStaticContext), 0);
        WritePatchPattern(Patterns::ParticlesMenu, ret_0, sizeof(ret_0), wstr(Patterns::ParticlesMenu), 0);
        WritePatchPattern_Int(sizeof(menu_mem_size), Patterns::MenuHeap_UsableMemorySize, (void*)menu_mem_size, wstr(Patterns::MenuHeap_UsableMemorySize), 8);
        WritePatchPattern_Int(sizeof(script_mem_size), Patterns::ScriptHeap_UsableMemorySize, (void*)script_mem_size, wstr(Patterns::ScriptHeap_UsableMemorySize), 8);
        WritePatchPattern_Int(sizeof(cpu_mem_size), Patterns::CPUHeap_UsableMemorySize, (void*)cpu_mem_size, wstr(Patterns::CPUHeap_UsableMemorySize), 8);
        WritePatchPattern(Patterns::Memory_ValidateContext, nop1x, sizeof(nop1x), wstr(Patterns::Memory_ValidateContext), 0x1d);
        WritePatchPattern(Patterns::Assert_UpdateSelectRegionByNameMenu, nop1x, sizeof(nop1x), wstr(Patterns::Assert_UpdateSelectRegionByNameMenu), 0);
        WritePatchPattern(Patterns::Assert_UpdateSelectIgcByNameMenu, nop1x, sizeof(nop1x), wstr(Patterns::Assert_UpdateSelectIgcByNameMenu), 0);
        WritePatchPattern(Patterns::Assert_UpdateSelectSpawnerByNameMenu, nop1x, sizeof(nop1x), wstr(Patterns::Assert_UpdateSelectSpawnerByNameMenu), 0);
    }
    if (bShowDebugConsole)
    {
        uintptr_t EvalScriptWarns = FindAndPrintPatternW(Patterns::GameWarnScriptPrint2, wstr(Patterns::GameWarnScriptPrint2));
        uintptr_t PrintAddr = FindAndPrintPatternW(Patterns::GameWarnScriptPrint, wstr(Patterns::GameWarnScriptPrint));
        if (EvalScriptWarns && PrintAddr)
        {
            uintptr_t JumpPattern = FindAndPrintPatternW(Patterns::Int3_14bytes, wstr(Patterns::Int3_14bytes));
            Make32to64Hook((void*)PrintAddr, (void*)JumpPattern, (void*)ScriptPrintWarn_CC, 6, wstr(PrintAddr), wstr(Patterns::Int3_14bytes), wstr(ScriptPrintWarn_CC));
            WritePatchPattern_Hook(Patterns::DoutMemPrint, 14, wstr(Patterns::DoutMemPrint), 0, (void*)ScriptPrintWarn_CC, nullptr);
            for (uint32_t i = 0; i < 2; i++)
            {
                uintptr_t DoutPrintfAddr = FindAndPrintPatternW(Patterns::DoutPrintf, wstr(Patterns::DoutPrintf));
                uintptr_t DebugOutputString1Addr = FindAndPrintPatternW(Patterns::DebugOutputString1, wstr(Patterns::DebugOutputString1));
                if (DoutPrintfAddr)
                {
                    Memory::DetourFunction64((void*)DoutPrintfAddr, (void*)printf_s, 14);
                }
                if (DebugOutputString1Addr)
                {
                    Memory::DetourFunction64((void*)DebugOutputString1Addr, (void*)printf_s, 14);
                }
            }
        }
        AllocConsole();
        freopen_s(&fGame, "CONOUT$", "w", stdout);
        freopen_s(&fGame, "CONOUT$", "w", stderr);
        freopen_s(&fGame, "CONOUT$", "w", stdin);
        printf_s(GIT_COMMIT "\n");
        printf_s(GIT_VER "\n");
        printf_s(GIT_NUM "\n");
        printf_s(BUILD_TIME "\n");
    }
}

void __stdcall Main()
{
    bLoggingEnabled = false;
    bDebugMenu = false;
    bShowDebugConsole = false;
    bExtendedDebugMenu = false;
    bTestMode = false;
    fDebugMenuSize = 0.6;
    wchar_t LogPath[_MAX_PATH]{};
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, L"" PROJECT_LOG_PATH);
    LoggingInit(L"" PROJECT_NAME, LogPath);
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
