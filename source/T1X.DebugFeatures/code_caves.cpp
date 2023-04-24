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
uint64_t ScriptLookupAddr = 0;

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

char temp_buffer[256];
const char* ScriptPrintWarn_CC(void* unused, char* fmt, ...)
{
    memset(temp_buffer, 0, sizeof(temp_buffer));
    va_list args;
    va_start(args, fmt);
    vsprintf_s(temp_buffer, sizeof(temp_buffer), fmt, args);
    va_end(args);
    printf_s(temp_buffer);
    return temp_buffer;
}

uint64_t AllocMemoryforStructureAddr = 0;
uint64_t CreateDevMenuStructureAddr = 0;
uint64_t AllocDevMenuMemoryforStructureAddr = 0;
uint64_t AllocDevMenu1Addr = 0;
uint64_t DevMenuCreateHeaderAddr = 0;
uint64_t DevMenuCreateEntryAddr = 0;
uint64_t DevMenuAddBoolAddr = 0;
uint64_t DevMenuAddFuncButtonAddr = 0;
uint64_t DevMenuAddIntSliderAddr = 0;
uint64_t MeleeMenuHook_ReturnAddr = 0;

bool test_boolean[10] = { false };
int32_t test_int = 100;
int32_t test_int2 = 10;
char menu_entry_text[128];
char BuildVer[128];

const char* AppendEllipsisToText(const char* text)
{
    memset(menu_entry_text, 0, sizeof(menu_entry_text));
    _snprintf_s(menu_entry_text, sizeof(menu_entry_text), "%s...", text);
    return menu_entry_text;
}

const char* FormatEntryText(const char* fmt, ...)
{
    memset(menu_entry_text, 0, sizeof(menu_entry_text));
    va_list args;
    va_start(args, fmt);
    vsprintf_s(menu_entry_text, sizeof(menu_entry_text), fmt, args);
    va_end(args);
    return menu_entry_text;
}

int32_t CrashTest_OnClick(DMenu_ClickStructure DMenu, int32_t click_mode);
int32_t SetPlayerHealth_OnClick(DMenu_ClickStructure DMenu, int32_t click_mode);

void MakeMeleeMenu(uintptr_t menu_structure)
{
    FUNCTION_PTR(uintptr_t, CreateDevMenuStructure_Caller, CreateDevMenuStructureAddr, uintptr_t menu_structure, uintptr_t last_menu_structure);
    FUNCTION_PTR(uintptr_t, AllocDevMenuMemoryforStructure_Caller, AllocDevMenuMemoryforStructureAddr, uint32_t menu_size, uint32_t alignment, const char* source_func, uint32_t source_line, const char* source_file);
    FUNCTION_PTR(void, AllocDevMenu1_Caller, AllocDevMenu1Addr, uintptr_t menu_structure_ptr, uint32_t unk, uint32_t structure_size);
    FUNCTION_PTR(uintptr_t, DevMenuCreateHeader_Caller, DevMenuCreateHeaderAddr, uintptr_t menu_structure_ptr, const char* title, uint32_t unk);
    FUNCTION_PTR(uintptr_t, DevMenuAddBool_Caller, DevMenuAddBoolAddr, uintptr_t menu_structure_ptr, const char* bool_name, bool* bool_var, const char* bool_description);
    FUNCTION_PTR(uintptr_t, DevMenuAddFuncButton_Caller, DevMenuAddFuncButtonAddr, uintptr_t menu_structure_ptr, const char* func_name, void* func_target, uint32_t arg, void* unk);
    FUNCTION_PTR(uintptr_t, DevMenuAddIntSlider_Caller, DevMenuAddIntSliderAddr, uintptr_t menu_structure_ptr, const char* int_name, int32_t* int_val, int32_t* step_val, DMenu_Int args, const char* int_description);
    FUNCTION_PTR(uintptr_t, DevMenuCreateEntry_Caller, DevMenuCreateEntryAddr, uintptr_t menu_structure_ptr, const char* name, uintptr_t last_menu_structure_ptr, uint32_t unk, uint32_t unk2, const char* entry_description);
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
    uint32_t int_button_size = 368;
    uintptr_t IntButton_ptr = AllocDevMenuMemoryforStructure_Caller(int_button_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    uintptr_t IntButton_structure = 0;
    if (IntButton_ptr)
    {
        DMenu_Int int_args;
        int_args.step_size = 10;
        int_args.min_val = test_int2;
        int_args.max_val = 1000;
        int_args.unk = 0;
        AllocDevMenu1_Caller(IntButton_ptr, 0, int_button_size);
        IntButton_structure = DevMenuAddIntSlider_Caller(IntButton_ptr, "Set fill ammo amount", &test_int2, &int_args.min_val, int_args, "For use with fill ammo");
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, IntButton_structure);
    uint32_t func_button_size = 200;
    uintptr_t FuncButton_ptr = AllocDevMenuMemoryforStructure_Caller(func_button_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    uintptr_t funcButton_structure = 0;
    if (FuncButton_ptr)
    {
        AllocDevMenu1_Caller(FuncButton_ptr, 0, func_button_size);
        funcButton_structure = DevMenuAddFuncButton_Caller(FuncButton_ptr, "Fill ammo", (void*)CrashTest_OnClick, 0, nullptr);
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, funcButton_structure);
    int_button_size = 368;
    IntButton_ptr = AllocDevMenuMemoryforStructure_Caller(int_button_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    IntButton_structure = 0;
    if (IntButton_ptr)
    {
        DMenu_Int int_args;
        int_args.step_size = 10;
        int_args.min_val = 10;
        int_args.max_val = 100;
        int_args.unk = 0;
        AllocDevMenu1_Caller(IntButton_ptr, 0, int_button_size);
        IntButton_structure = DevMenuAddIntSlider_Caller(IntButton_ptr, "Set health amount", &test_int, &int_args.min_val, int_args, "For use with set player health");
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, IntButton_structure);
    func_button_size = 200;
    FuncButton_ptr = AllocDevMenuMemoryforStructure_Caller(func_button_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    funcButton_structure = 0;
    if (FuncButton_ptr)
    {
        AllocDevMenu1_Caller(FuncButton_ptr, 0, func_button_size);
        funcButton_structure = DevMenuAddFuncButton_Caller(FuncButton_ptr, "Set player health", (void*)SetPlayerHealth_OnClick, 0, nullptr);
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, funcButton_structure);
    for (uint32_t i = 0; i < sizeof(test_boolean); i++)
    {
        // Create the bool
        uint32_t bool_menu_size = 192;
        uintptr_t FirstBool_ptr = AllocDevMenuMemoryforStructure_Caller(header_menu_size, 16, __FUNCSIG__, __LINE__, __FILE__);
        uintptr_t BoolPtr = 0;
        if (FirstBool_ptr)
        {
            AllocDevMenu1_Caller(FirstBool_ptr, 0, bool_menu_size);
            BoolPtr = DevMenuAddBool_Caller(FirstBool_ptr, FormatEntryText("Test Bool %u", i + 1), &test_boolean[i], nullptr);
        }
        CreateDevMenuStructure_Caller(SubHeaderPtr, BoolPtr);
    }
    // Create the entry point
    uint32_t entry_menu_size = 200;
    uintptr_t entry_ptr = AllocDevMenuMemoryforStructure_Caller(entry_menu_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    if (entry_ptr)
    {
        AllocDevMenu1_Caller(entry_ptr, 0, entry_menu_size);
        DevMenuCreateEntry_Caller(entry_ptr, AppendEllipsisToText(MyMenuEntryText), Header_ptr, 0, 0, BuildVer);
    }
    CreateDevMenuStructure_Caller(menu_structure, entry_ptr);
    memset(menu_entry_text, 0, sizeof(menu_entry_text));
}

int32_t SetPlayerHealth_OnClick(DMenu_ClickStructure DMenu, int32_t click_mode)
{
    if (click_mode == 5)
    {
        FUNCTION_PTR(uintptr_t, ScriptManager_LookupClass, ScriptLookupAddr, StringId64 sid, uint32_t unk);
        DMenu.DMENU_ARG = uint64_t(test_int);
        printf_s(
                        str(click_mode)": %i\n"
                        str(&DMenu)": 0x%p\n"
                        str(DMenu.DMENU_TEXT)": %s\n"
                        str(DMenu.DMENU_ARG)": 0x%016llx\n"
                        str(DMenu.DMENU_FUNC)": 0x%016llx\n",
                        click_mode, &DMenu, DMenu.DMENU_TEXT, DMenu.DMENU_ARG, DMenu.DMENU_FUNC);
        const char* native_name = "set-player-health";
        StringId64 native_hash = SID(native_name);
        uintptr_t LookupPtr = ScriptManager_LookupClass(native_hash, 1);
        if (LookupPtr)
        {
            LookupPtr = *reinterpret_cast<uintptr_t*>(LookupPtr + 8);
            printf_s("#%.16llx (%s) found at 0x%016llx\n", native_hash, native_name, LookupPtr);
            struct health_args
            {
                int32_t health = test_int;
            };
            health_args _health_args;
            FUNCTION_PTR(void, NativeFunc, LookupPtr, health_args* args, uint32_t argc, int32_t * argv_first);
            uint32_t argc = 1;
            printf_s("Firing #%.16llx (%s) with %u arguments\n", native_hash, native_name, argc);
            NativeFunc(&_health_args, argc, &_health_args.health);
            return 1;
        }
        else
        {
            printf_s("Can't find #%.16llx (%s)!\n", native_hash, native_name);
            return 0;
        }
    }
    return 0;
}

int32_t CrashTest_OnClick(DMenu_ClickStructure DMenu, int32_t click_mode)
{
    if (click_mode == 5)
    {
        FUNCTION_PTR(uintptr_t, ScriptManager_LookupClass, ScriptLookupAddr, StringId64 sid, uint32_t unk);
        DMenu.DMENU_ARG = uint64_t(test_int2);
        printf_s(
                        str(click_mode)": %i\n"
                        str(&DMenu)": 0x%p\n"
                        str(DMenu.DMENU_TEXT)": %s\n"
                        str(DMenu.DMENU_ARG)": 0x%016llx\n"
                        str(DMenu.DMENU_FUNC)": 0x%016llx\n",
                        click_mode, &DMenu, DMenu.DMENU_TEXT, DMenu.DMENU_ARG, DMenu.DMENU_FUNC);
        const char* native_name = "add-player-weapon-reserve-ammo";
        StringId64 native_hash = SID(native_name);
        uintptr_t LookupPtr = ScriptManager_LookupClass(native_hash, 1);
        if (LookupPtr)
        {
            LookupPtr = *reinterpret_cast<uintptr_t*>(LookupPtr + 8);
            printf_s("#%.16llx (%s) found at 0x%016llx\n", native_hash, native_name, LookupPtr);
            struct ammo_args
            {
                int32_t ammo_count = test_int2;
            };
            ammo_args _ammo_args;
            FUNCTION_PTR(void, NativeFunc, LookupPtr, ammo_args *args, uint32_t argc, int32_t *argv_first);
            uint32_t argc = 1;
            printf_s("Firing #%.16llx (%s) with %u arguments\n", native_hash, native_name, argc);
            NativeFunc(&_ammo_args, argc, &_ammo_args.ammo_count);
            return 1;
        }
        else
        {
            printf_s("Can't find #%.16llx (%s)!\n", native_hash, native_name);
        }
    }
    return 0;
}
