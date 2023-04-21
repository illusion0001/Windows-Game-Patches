#include "code_caves.hpp"
#include "memory.hpp"

uint64_t Memory_PushAllocatorReturnAddr = 0;
void __attribute__((naked)) Memory_PushAllocator_CC() {
    __asm {
        // code by infogram
        cmp eax, 0x10 // Debug DMENU
        je retail_memory_type
            // extra check
        cmp eax, 0xa  // Debug CPU Memory
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

const char* weapon_list_main[] = {"invisible-cube",
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
                                  "t1x-leather-toolkit"};

const char* weapon_list_subsection[] = {"invisible-cube",
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
                                        "t1x-switchblade"};

const char* weapon_subentry_names[] = {"Misc", "Consumables", "Throwables", "Ellie", "Joel", "Melee"};

const constexpr uint32_t GivePlayerWeapon_ListMax = sizeof(weapon_list_main) / sizeof(weapon_list_main[0]);
const constexpr uint32_t GivePlayerWeapon_SubListMax = sizeof(weapon_list_subsection) / sizeof(weapon_list_subsection[0]);
const constexpr uint32_t GivePlayerWeapon_EntryListMax = sizeof(weapon_subentry_names) / sizeof(weapon_subentry_names[0]);

// Main list
uint32_t GivePlayerWeapon_index_count = 0;
uint64_t GivePlayerWeapon_MainReturn = 0;
// Sub entry
uint32_t GivePlayerWeaponSub_index_count = 0;
uint64_t GivePlayerWeapon_SubReturn = 0;
// Entry
uint32_t GivePlayerWeaponEntry_index_count = 0;
uint64_t GivePlayerWeapon_EntryReturn = 0;

// Game funcs
uint64_t Game_SnprintfAddr = 0;
uint64_t GamePrintf = 0;
uint64_t ScriptLookupAddr = 0;

uint64_t AllocMemoryforStructureAddr = 0;
uint64_t CreateDevMenuStructureAddr = 0;
uint64_t AllocDevMenuMemoryforStructureAddr = 0;
uint64_t AllocDevMenu1Addr = 0;
uint64_t DevMenuCreateHeaderAddr = 0;
uint64_t DevMenuCreateEntryAddr = 0;
uint64_t DevMenuAddBoolAddr = 0;
uint64_t MeleeMenuHook_ReturnAddr = 0;

char temp_str[128];

const char* GivePlayerWeaponMain(const StringId64 Sid, const int32_t mode)
{
    const char* string_index = nullptr;
    switch (mode) {
    case 1: {
        if (GivePlayerWeapon_index_count == GivePlayerWeapon_ListMax)
            GivePlayerWeapon_index_count = 0;
        string_index = weapon_list_main[GivePlayerWeapon_index_count];
        GivePlayerWeapon_index_count++;
        break;
    }
    case 2: {
        if (GivePlayerWeaponSub_index_count == GivePlayerWeapon_SubListMax)
            GivePlayerWeaponSub_index_count = 0;
        string_index = weapon_list_subsection[GivePlayerWeaponSub_index_count];
        GivePlayerWeaponSub_index_count++;
        break;
    }
    case 3: {
        if (GivePlayerWeaponEntry_index_count == GivePlayerWeapon_EntryListMax)
            GivePlayerWeaponEntry_index_count = 0;
        string_index = weapon_subentry_names[GivePlayerWeaponEntry_index_count];
        GivePlayerWeaponEntry_index_count++;
        break;
    }
    default: {
        memset(temp_str, 0, sizeof(temp_str));
        return temp_str;
    }
    }
    if (string_index[0] == '\0')
        string_index = "(null)";
    _snprintf_s(temp_str, sizeof(temp_str), "%s (#%.16llx)", string_index, Sid);
    return temp_str;
}

void __attribute__((naked)) GivePlayerWeapon_MainCC() {
    __asm {
        mov rcx, [r15 + 0x8]
        mov rcx, [rsi + rcx * 1]
        mov edx, 1
        call [GivePlayerWeaponMain]
        jmp [GivePlayerWeapon_MainReturn]
    }
}

void __attribute__((naked)) GivePlayerWeapon_SubCC() {
    __asm {
        mov rcx, [r13 + 0x8]
        mov rcx, [r12 + rcx * 1]
        mov edx, 2
        call [GivePlayerWeaponMain]
        jmp [GivePlayerWeapon_SubReturn]
    }
}

void __attribute__((naked)) GivePlayerWeapon_EntryCC() {
    __asm {
        mov rcx, [r14 + 0x8]
        mov rcx, [r15 + rcx * 1]
        mov edx, 3
        call [GivePlayerWeaponMain]
        jmp [GivePlayerWeapon_EntryReturn]
    }
}

#define FUNCTION_PTR(return_type, func_name, func_addr, ...) \
    typedef return_type (*func_name##_t)(__VA_ARGS__); \
    func_name##_t func_name = (func_name##_t)(func_addr);

char temp_buffer[256];
int32_t ScriptPrintWarn_CC(void* unused, char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf_s(temp_buffer, sizeof(temp_buffer), fmt, args);
    va_end(args);
    FUNCTION_PTR(int, GamePrintf_Caller, GamePrintf, const char* fmt, ...)
    GamePrintf_Caller(temp_buffer);
    memset(temp_buffer, 0, sizeof(temp_buffer));
    return 0;
}

bool* test_boolean;
bool* test_boolean2;

char menu_entry_text[128];
const char* AppendEllipsisToText(const char* text)
{
    memset(menu_entry_text, 0, sizeof(menu_entry_text));
    _snprintf_s(menu_entry_text, sizeof(menu_entry_text), "%s...", text);
    return menu_entry_text;
}

void MakeMeleeMenu(uintptr_t menu_structure, uintptr_t last_menu_structure)
{
    FUNCTION_PTR(uintptr_t, CreateDevMenuStructure_Caller, CreateDevMenuStructureAddr, uintptr_t menu_structure, uintptr_t last_menu_structure)
    FUNCTION_PTR(uintptr_t, AllocDevMenuMemoryforStructure_Caller, AllocDevMenuMemoryforStructureAddr, uint32_t menu_size, uint32_t alignment, const char* source_func, uint32_t source_line, const char* source_file)
    FUNCTION_PTR(void, AllocDevMenu1_Caller, AllocDevMenu1Addr, uintptr_t menu_structure_ptr, uint32_t unk, uint32_t structure_size)
    FUNCTION_PTR(uintptr_t, DevMenuCreateHeader_Caller, DevMenuCreateHeaderAddr, uintptr_t menu_structure_ptr, const char* title, uint32_t unk)
    FUNCTION_PTR(uintptr_t, DevMenuAddBool_Caller, DevMenuAddBoolAddr, uintptr_t menu_structure_ptr, const char* bool_name, bool** bool_var, const char* bool_subtitle)
    FUNCTION_PTR(uintptr_t, DevMenuCreateEntry_Caller, DevMenuCreateEntryAddr, uintptr_t menu_structure_ptr, const char* name, uintptr_t last_menu_structure_ptr, uint32_t unk, uint32_t unk2, const char* subtitle)
    CreateDevMenuStructure_Caller(menu_structure, last_menu_structure);
    // Create the header
    uint32_t header_menu_size = 224;
    uintptr_t Header_ptr = AllocDevMenuMemoryforStructure_Caller(header_menu_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    uintptr_t SubHeaderPtr = 0;
    const char* MyMenuEntryText = "Example Menu";
    if (Header_ptr)
    {
        AllocDevMenu1_Caller(Header_ptr, 0, header_menu_size);
        SubHeaderPtr = DevMenuCreateHeader_Caller(Header_ptr, MyMenuEntryText, 0);
    }
#if 1
    // Create the bool
    uint32_t bool_menu_size = 192;
    uintptr_t FirstBool_ptr = AllocDevMenuMemoryforStructure_Caller(header_menu_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    uintptr_t BoolPtr = 0;
    if (FirstBool_ptr)
    {
        AllocDevMenu1_Caller(FirstBool_ptr, 0, bool_menu_size);
        BoolPtr = DevMenuAddBool_Caller(FirstBool_ptr, "Test Boolean", &test_boolean, nullptr);
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, BoolPtr);
    // Create the second bool
    FirstBool_ptr = AllocDevMenuMemoryforStructure_Caller(header_menu_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    BoolPtr = 0;
    if (FirstBool_ptr)
    {
        AllocDevMenu1_Caller(FirstBool_ptr, 0, bool_menu_size);
        BoolPtr = DevMenuAddBool_Caller(FirstBool_ptr, "Test Boolean2", &test_boolean2, nullptr);
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, BoolPtr);
#endif
    // Create the entry point
    uint32_t entry_menu_size = 200;
    uintptr_t entry_ptr = AllocDevMenuMemoryforStructure_Caller(entry_menu_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    if (entry_ptr)
    {
        AllocDevMenu1_Caller(entry_ptr, 0, entry_menu_size);
        DevMenuCreateEntry_Caller(entry_ptr, AppendEllipsisToText(MyMenuEntryText), Header_ptr, 0, 0, "Example Subtitles");
    }
    memset(menu_entry_text, 0, sizeof(menu_entry_text));
}

void __attribute__((naked)) MakeMeleeMenu_CC()
{
    __asm
    {
        CALL MakeMeleeMenu
        JMP [MeleeMenuHook_ReturnAddr]
    }
}

int32_t CrashTest_OnClick(DMenu_ClickStructure DMenu, int32_t click_mode)
{
    if (click_mode == 5)
    {
        FUNCTION_PTR(int, GamePrintf_Caller, GamePrintf, const char* fmt, ...)
        // uintptr_t (*ScriptManager_LookupClass) (StringId64 sid, int unk) = ((uintptr_t(*) (StringId64 sid, int unk)) (ScriptLookupAddr));
        FUNCTION_PTR(uintptr_t, ScriptManager_LookupClass, ScriptLookupAddr, StringId64 sid, uint32_t unk)
        GamePrintf_Caller(
                        str(click_mode)": %i\n"
                        str(&DMenu)": 0x%p\n"
                        str(DMenu.DMENU_TEXT)": %s\n"
                        str(DMenu.DMENU_ARG)": 0x%016llx\n"
                        str(DMenu.DMENU_FUNC)": 0x%016llx\n",
                        click_mode, &DMenu, DMenu.DMENU_TEXT, DMenu.DMENU_ARG, DMenu.DMENU_FUNC);
        const char* native_name = "add-player-weapon-reserve-ammo";
        StringId64 native_hash = ToStringId64(native_name);
        uintptr_t LookupPtr = ScriptManager_LookupClass(native_hash, 1);
        if (LookupPtr)
        {
            LookupPtr = LookupPtr + 8;
            LookupPtr = Memory::ReadMultiLevelPointer(LookupPtr, { 0x0 });
            GamePrintf_Caller("#%.16llx (%s) found at 0x%016llx\n", native_hash, native_name, LookupPtr);
            // void(*NativeFunc) (uint64_t argv[], uint64_t argc, uintptr_t *argv_first) = ((void(*) (uint64_t argv[], uint64_t argc, uintptr_t *argv_first)) (LookupPtr));
            FUNCTION_PTR(void, NativeFunc, LookupPtr, uint64_t argv[], uint64_t argc, uintptr_t *argv_first)
            uint64_t argv_test[] = { 32767 };
            uint32_t argc = sizeof(argv_test) / sizeof(argv_test[0]);
            GamePrintf_Caller("Firing #%.16llx (%s) with %u arguments\n", native_hash, native_name, argc);
            NativeFunc(argv_test, argc, &argv_test[0]);
            return 1;
        }
        else
        {
            GamePrintf_Caller("Can't find #%.16llx (%s)!\n", native_hash, native_name);
        }
    }
    return 0;
}
