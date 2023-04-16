#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

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

#define MEM_TYPE 0x25 // TODO: Check this heap type

uint64_t Memory_PushAllocatorReturnAddr = 0;
void __attribute__((naked)) Memory_PushAllocator_CC()
{
    __asm
    {
        // code by infogram
        cmp eax, 0x10
        je retail_memory_type
        // extra check
        cmp eax, 0xa
        jnz ret_loc
        retail_memory_type :
        mov eax, MEM_TYPE
        mov dword ptr [rcx], eax
        // original code
        ret_loc :
        lea rcx, [rsp + 0x30]
        mov dword ptr[rsp + 0x30], eax
        mov r15, r9
        mov r14d, r8d
        jmp[Memory_PushAllocatorReturnAddr]
    }
}

const wchar_t* DevMenu_MenuSize = L"c7 41 ?? 9a 99 19 3f 49 8b d1 c6 41 ?? 00 44 89 51 ??";
const wchar_t* ConsoleOutput = L"0f 45 ce 88 0d ?? ?? ?? ?? 48 8d ?? ?? ?? ?? ?? e8 ?? ?? ?? ??";
const wchar_t* m_onDisc_DevMenu = L"8a 8f ?? ?? ?? ?? 84 c9 0f 94 c2 84 c9 0f 95 c1";
// Give Player Weapons
const wchar_t* GivePlayerWeapon_Main = L"49 8b 4f 08 48 8d 54 24 30 41 b0 01 c7 44 24 30 ?? ?? ?? ?? 48 8b 0c 0e e8 ?? ?? ?? ??";
const wchar_t* GivePlayerWeapon_SubSection = L"49 8b 4d 08 48 8d 54 24 40 41 b0 01 49 8b 0c 0c e8 ?? ?? ?? ??";
const wchar_t* GivePlayerWeapon_Entry = L"49 8b 4e 08 48 8d 54 24 30 41 b0 01 49 8b 0c 0f e8 ?? ?? ?? ??";
const wchar_t* Assert_LevelDef_LevelManifst = L"c7 44 24 20 f3 04 00 00 4c 8d 05";
// thanks to infogram for offsets
const wchar_t* Memory_isDebugMemoryAval = L"32 c0 c3 cc 48 8b 41 08 c3";
const wchar_t* Memory_PushAllocatorJMPAddr = L"48 8d 4c 24 30 89 44 24 30 4d 8b f9 45 8b f0 48 8b ea e8 ?? ?? ?? ??";
const wchar_t* DebugDrawStaticContext = L"e8 ?? ?? ?? ?? 33 d2 33 c9 e8 ?? ?? ?? ?? 80 b8 3d 3f 00 00 00 75 ??";
const wchar_t* MaterialDebug_Heap = L"c7 44 24 ?? 10 00 00 00 b9 18 00 00 00 48 8d 54 24 ?? e8 ?? ?? ?? ??";
const wchar_t* ParticlesMenu = L"40 56 48 83 ec 30 48 8b f1 33 c9 e8 ?? ?? ?? ?? 84 c0 0f 84 ?? ?? ?? ?? 48 89 5c 24 40 4c 8d 0d ?? ?? ?? ??";
const wchar_t* UpdateSelectRegionByNameMenu_Heap = L"b9 08 40 02 00 c7 44 24 ?? 0a 00 00 00 48 8d 54 24 ??";
const wchar_t* UpdateSelectSpawner = L"c7 44 24 ?? 0a 00 00 00 48 8d 54 24 ?? 88 5c 24 ?? b9 08 20 04 00 e8 ?? ?? ?? ??";
const wchar_t* Memory_ValidateContext = L"cc 48 85 db 0f 84 ?? ?? ?? ?? 33 c9 ff d3 90 e9 ?? ?? ?? ?? 48 8b 43 08 49 39 47 08 75 ?? 48 8b 43 10 49 39 47 10 0f 84 ?? ?? ?? ??";
const wchar_t* MenuHeap_UsableMemorySize = L"80 00 00 00 05 00 00 00 00 00 ?? 00 00 00 00 00 00 40 00 00"; // ALLOCATION_MENU_HEAP
const wchar_t* ScriptHeap_UsableMemorySize = L"5e 00 00 00 05 00 00 00 00 00 ?? 00 00 00 00 00 00 40 00 00"; // ALLOCATION_SCRIPT_HEAP
const wchar_t* CPUHeap_UsableMemorySize = L"03 00 00 00 02 00 00 00 00 00 ?? ?? 00 00 00 00 00 00 10 00"; // ALLOCATION_CPU_MEMORY
// inline asserts
// based on source lines
const wchar_t* Assert_UpdateSelectRegionByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 8e 00 00 00";
const wchar_t* Assert_UpdateSelectIgcByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? bb 10 00 00 00 41 b9 84 03 00 00";
const wchar_t* Assert_UpdateSelectSpawnerByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 a3 00 00 00";

const char* weapon_list_main[] = {
                                    "invisible-cube",
                                    "pistol-shotgun-military",
                                    "bloater-pustule",
                                    "pistol-colt-1911-dark",
                                    "bow-joel",
                                    "shiv-d",
                                    "invisible-cube",
                                    "bandage-autofeed",
                                    "rifle-vepr",
                                    "pistol-uber-hunter",
                                    "invisible-cube",
                                    "rifle-m14-sniper",
                                    "bow-ellie",
                                    "invisible-cube",
                                    "invisible-cube",
                                    "bow-ellie",
                                    "pistol-revolver-taurus",
                                    "rifle-m14-sniper",
                                    "bow-scar",
                                    "rifle-mpx5",
                                    "two-hand-axe",
                                    "molotov",
                                    "digital-recorder-carry-t1x",
                                    "club-2h-scar",
                                    "rifle-remington-81",
                                    "hammer-modern",
                                    "pipe-bomb",
                                    "pistol-9mm-firefly",
                                    "gas-mask-lev",
                                    "mp-spotting-scope",
                                    "pistol-revolver-taurus",
                                    "rifle-m14-sniper",
                                    "shotgun-pump-stock",
                                    "pipe-wrench",
                                    "shotgun-remington-pump",
                                    "t1x-shotgun-remington-pump",
                                    "hammer-scar",
                                    "rifle-mini-14-manny",
                                    "flamethrower",
                                    "invisible-cube",
                                    "club-1h-police",
                                    "baseball-bat",
                                    "invisible-cube",
                                    "shiv-b",
                                    "invisible-cube",
                                    "shotgun-pump-firefly",
                                    "pistol-browning",
                                    "rifle-mini-14",
                                    "shotgun-pump-stock",
                                    "baseball-bat-metal",
                                    "bow-lev",
                                    "pistol-shotgun-firefly",
                                    "pistol-uber-firefly",
                                    "m4-rifle-military",
                                    "pickaxe-scar-player",
                                    "ski-aquarium-dog-toy",
                                    "knife-bill-game",
                                    "hatchet-tomahawk",
                                    "short-pipe",
                                    "pistol-revolver-taurus-owen",
                                    "pistol-revolver-scar",
                                    "stun-bomb",
                                    "pistol-ruger",
                                    "rifle-galil",
                                    "rifle-m14-sniper",
                                    "invisible-cube",
                                    "m4-rifle-firefly",
                                    "torch-game",
                                    "invisible-cube",
                                    "bow-ellie",
                                    "molotov",
                                    "machete-wood-joel",
                                    "pistol-xcaliber",
                                    "invisible-cube",
                                    "shiv-f",
                                    "t1x-leather-toolkit",
                                    "pistol-colt-defender",
                                    "shotgun-remington-pump-sawedoff",
                                    "two-hand-axe-scar",
                                    "shotgun-dbl-barrel-scar",
                                    "rifle-vepr",
                                    "rifle-m14-dark",
                                    "rifle-mini-14-wood",
                                    "club-1h-scar",
                                    "bandage-autofeed",
                                    "bow-scar",
                                    "rifle-bolt-firefly",
                                    "pistol-cz75",
                                    "shiv-a",
                                    "pistol-revolver-taurus-owen",
                                    "pistol-9mm-military",
                                    "pistol-beretta",
                                    "invisible-cube",
                                    "invisible-cube",
                                    "invisible-cube",
                                    "fob-basketball",
                                    "rifle-bolt-m24",
                                    "pistol-ruger",
                                    "throw-brick",
                                    "NO_ART_GROUP",
                                    "machete-black",
                                    "bloater-pustule",
                                    "rifle-ruger-scar",
                                    "smoke-bomb",
                                    "machete-scar",
                                    "rifle-winchester-lever",
                                    "shotgun-remington-pump-sawedoff",
                                    "lead-pipe",
                                    "pistol-ruger",
                                    "t1x-shiv-mod-a",
                                    "pistol-shotgun-hunter",
                                    "amp-gift-shop-toy-orca",
                                    "pistol-revolver-firefly",
                                    "molotov",
                                    "smoke-bomb-basic",
                                    "beer-bottle",
                                    "machete-black",
                                    "shotgun-pump-stock-black",
                                    "pipe-bomb",
                                    "crossbow",
                                    "bloater-pustule",
                                    "shotgun-baserri-over-under",
                                    "bloater-pustule",
                                    "pistol-colt-1911-silver",
                                    "pickaxe-scar",
                                    "shotgun-remington-pump",
                                    "rifle-m14-sniper",
                                    "bow-toy",
                                    "shiv-e",
                                    "rifle-sniper-hunter",
                                    "pistol-revolver-357",
                                    "syringe",
                                    "rifle-remington-81",
                                    "scythe-scar",
                                    "hatchet-black",
                                    "crowbar-one-hand",
                                    "hatchet-modern",
                                    "shotgun-remington-pump-black",
                                    "dagger-scar-lev",
                                    "trap-mine",
                                    "invisible-cube",
                                    "rifle-remington-81",
                                    "two-by-four",
                                    "shotgun-dbl-barrel-abby",
                                    "rifle-bolt-m24-dark",
                                    "firecracker",
                                    "oil-can-silencer",
                                    "shotgun-remington-pump",
                                    "rifle-mpx5-tilt",
                                    "short-pipe",
                                    "t1x-turret-sniper",
                                    "t1x-shiv",
                                    "molotov",
                                    "whistle",
                                    "rifle-mpx5",
                                    "dagger-scar",
                                    "NO_ART_GROUP",
                                    "NO_ART_GROUP",
                                    "pistol-glock",
                                    "hatchet-tomahawk",
                                    "machete-wood",
                                    "pistol-cz75-black",
                                    "rifle-bolt-m24",
                                    "pat-snowball",
                                    "pistol-browning-scar",
                                    "sledgehammer",
                                    "fireman-axe",
                                    "shiv-c",
                                    "pistol-revolver-militia",
                                    "pistol-revolver-military",
                                    "t1x-shiv-mod-b",
                                    "pistol-9mm-hunter",
                                    "rifle-semi-auto-firefly",
                                    "rifle-bolt-scar",
                                    "hatchet-scar",
                                    "rifle-remington-bolt",
                                    "tennis-ball-throw",
                                    "t1x-leather-toolkit" };

const char* weapon_list_subsection[] = {
                                        "invisible-cube",
                                        "",
                                        "bandage-autofeed",
                                        "bandage-autofeed",
                                        "throw-brick",
                                        "beer-bottle",
                                        "nail-bomb-basic",
                                        "molotov-basic",
                                        "smoke-bomb-basic",
                                        "t1x-mal-gas-can",
                                        "pistol-t1x-beretta-jaguar-pistol",
                                        "t1x-bow-ellie",
                                        "t1x-rifle-bolt-m24",
                                        "rifle-shotgun-pump-hunter",
                                        "pistol-revolver-hunter",
                                        "rifle-bolt-hunter",
                                        "t1x-super-soaker",
                                        "t1x-pistol-9mm",
                                        "t1x-taurus-66b4-revolver",
                                        "t1x-pistol-uber-t1",
                                        "t1x-pistol-shotgun",
                                        "t1x-rifle-bolt",
                                        "t1x-m4-rifle-t1",
                                        "t1x-shotgun-pump-stock",
                                        "t1x-bow-dark",
                                        "t1x-flamethrower",
                                        "t1x-super-soaker",
                                        "t1x-machete-blade",
                                        "t1x-hatchet",
                                        "t1x-baseball-bat",
                                        "t1x-two-by-four",
                                        "t1x-lead-pipe",
                                        "invisible-cube",
                                        "t1x-switchblade" };

const char* weapon_subentry_names[] = {
                                        "Misc",
                                        "Consumables",
                                        "Throwables",
                                        "Ellie",
                                        "Joel",
                                        "Melee" };

#define GIVE_WEAPON_MAIN_MAX 178
#define GIVE_WEAPON_SUB_MAX 34
#define GIVE_WEAPON_ENTRY_MAX 6

// Main list
uint64_t GivePlayerWeapon_ptr_offset = 0;
uint64_t GivePlayerWeapon_index_count = 0;
uint64_t GivePlayerWeapon_MainReturn = 0;
// Sub entry
uint64_t GivePlayerWeaponSub_ptr_offset = 0;
uint64_t GivePlayerWeaponSub_index_count = 0;
uint64_t GivePlayerWeapon_SubReturn = 0;
// Entry
uint64_t GivePlayerWeaponEntry_ptr_offset = 0;
uint64_t GivePlayerWeaponEntry_index_count = 0;
uint64_t GivePlayerWeapon_EntryReturn = 0;

void __attribute__((naked)) GivePlayerWeapon_MainCC()
{
    __asm
    {
        mov r8, [GivePlayerWeapon_index_count]
        cmp r8, GIVE_WEAPON_MAIN_MAX
        mov rdx, 1
        mov r9, 8
        jz reset_idx
        jnz set_ptr
        reset_idx :
        mov rax, 0
        mov[GivePlayerWeapon_index_count], rax
        mov[GivePlayerWeapon_ptr_offset], rax
        set_ptr:
        lea rax, [weapon_list_main]
        add rax, [GivePlayerWeapon_ptr_offset]
        mov rax, [rax]
        add[GivePlayerWeapon_ptr_offset], r9
        add[GivePlayerWeapon_index_count], rdx
        jmp[GivePlayerWeapon_MainReturn]
    }
}

void __attribute__((naked)) GivePlayerWeapon_SubCC()
{
    __asm
    {
        mov r8, [GivePlayerWeaponSub_index_count]
        cmp r8, GIVE_WEAPON_SUB_MAX
        mov rdx, 1
        mov r9, 8
        jz reset_idx
        jnz set_ptr
        reset_idx :
        mov rax, 0
        mov[GivePlayerWeaponSub_index_count], rax
        mov[GivePlayerWeaponSub_ptr_offset], rax
        set_ptr :
        lea rax, [weapon_list_subsection]
        add rax, [GivePlayerWeaponSub_ptr_offset]
        mov rax, [rax]
        add[GivePlayerWeaponSub_ptr_offset], r9
        add[GivePlayerWeaponSub_index_count], rdx
        jmp[GivePlayerWeapon_SubReturn]
    }
}

void __attribute__((naked)) GivePlayerWeapon_EntryCC()
{
    __asm
    {
        mov r8, [GivePlayerWeaponEntry_index_count]
        cmp r8, GIVE_WEAPON_ENTRY_MAX
        mov rdx, 1
        mov r9, 8
        jz reset_idx
        jnz set_ptr
        reset_idx :
        mov rax, 0
        mov[GivePlayerWeaponEntry_index_count], rax
        mov[GivePlayerWeaponEntry_ptr_offset], rax
        set_ptr :
        lea rax, [weapon_subentry_names]
        add rax, [GivePlayerWeaponEntry_ptr_offset]
        mov rax, [rax]
        add[GivePlayerWeaponEntry_ptr_offset], r9
        add[GivePlayerWeaponEntry_index_count], rdx
        jmp[GivePlayerWeapon_EntryReturn]
    }
}

void ApplyDebugPatches(void)
{
    if (bDebugMenu)
    {
        if (fDebugMenuSize > 1)
            fDebugMenuSize = 1.0;
        else if (fDebugMenuSize < 0.4)
            fDebugMenuSize = 0.4;
        WritePatchPattern_Int(4, DevMenu_MenuSize, (void*)*(uint32_t*)&fDebugMenuSize, wstr(DevMenu_MenuSize), 3);
        const unsigned char mov_ecx_0[] = { 0xb9, 0x00, 0x00, 0x00, 0x00, 0x90 };
        const unsigned char nop1x[] = { 0x90 };
        WritePatchPattern(m_onDisc_DevMenu, mov_ecx_0, sizeof(mov_ecx_0), wstr(m_onDisc_DevMenu), 0);
        WritePatchPattern(Assert_LevelDef_LevelManifst, nop1x, sizeof(nop1x), wstr(Assert_LevelDef_LevelManifst), 30);
        WritePatchPattern_Hook(GivePlayerWeapon_Main, 29, wstr(GivePlayerWeapon_Main), 0, &GivePlayerWeapon_MainCC, &GivePlayerWeapon_MainReturn);
        WritePatchPattern_Hook(GivePlayerWeapon_SubSection, 21, wstr(GivePlayerWeapon_SubSection), 0, &GivePlayerWeapon_SubCC, &GivePlayerWeapon_SubReturn);
        WritePatchPattern_Hook(GivePlayerWeapon_Entry, 21, wstr(GivePlayerWeapon_Entry), 0, &GivePlayerWeapon_EntryCC, &GivePlayerWeapon_EntryReturn);
    }
    if (bShowDebugConsole)
    {
        const unsigned char mov_cl_1_nop[] = { 0xb1, 0x01, 0x90 };
        WritePatchPattern(ConsoleOutput, mov_cl_1_nop, sizeof(mov_cl_1_nop), wstr(ConsoleOutput), 0);
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
        WritePatchPattern_Hook(Memory_PushAllocatorJMPAddr, 15, wstr(Memory_PushAllocatorJMPAddr), 0, &Memory_PushAllocator_CC, &Memory_PushAllocatorReturnAddr);
        WritePatchPattern(Memory_isDebugMemoryAval, ret_1_al, sizeof(ret_1_al), wstr(Memory_isDebugMemoryAval), 0);
        WritePatchPattern(DebugDrawStaticContext, nop5x, sizeof(nop5x), wstr(DebugDrawStaticContext), 0);
        WritePatchPattern(MaterialDebug_Heap, heap_type, sizeof(heap_type), wstr(MaterialDebug_Heap), 4);
        WritePatchPattern(ParticlesMenu, ret_0, sizeof(ret_0), wstr(ParticlesMenu), 0);
        WritePatchPattern(UpdateSelectRegionByNameMenu_Heap, heap_type, sizeof(heap_type), wstr(UpdateSelectRegionByNameMenu_Heap), 9);
        WritePatchPattern(UpdateSelectSpawner, heap_type, sizeof(heap_type), wstr(UpdateSelectSpawner), 4);
        WritePatchPattern_Int(8, MenuHeap_UsableMemorySize, (void*)menu_mem_size, wstr(MenuHeap_UsableMemorySize), 8);
        WritePatchPattern_Int(8, ScriptHeap_UsableMemorySize, (void*)script_mem_size, wstr(ScriptHeap_UsableMemorySize), 8);
        WritePatchPattern_Int(8, CPUHeap_UsableMemorySize, (void*)cpu_mem_size, wstr(CPUHeap_UsableMemorySize), 8);
        WritePatchPattern(Memory_ValidateContext, nop1x, sizeof(nop1x), wstr(Memory_ValidateContext), 0);
        WritePatchPattern(Assert_UpdateSelectRegionByNameMenu, nop1x, sizeof(nop1x), wstr(Assert_UpdateSelectRegionByNameMenu), 0);
        WritePatchPattern(Assert_UpdateSelectIgcByNameMenu, nop1x, sizeof(nop1x), wstr(Assert_UpdateSelectIgcByNameMenu), 0);
        WritePatchPattern(Assert_UpdateSelectSpawnerByNameMenu, nop1x, sizeof(nop1x), wstr(Assert_UpdateSelectSpawnerByNameMenu), 0);
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
