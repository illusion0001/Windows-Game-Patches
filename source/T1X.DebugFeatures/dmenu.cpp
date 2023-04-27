#include "dmenu.hpp"

uint64_t AllocMemoryforStructureAddr = 0;
uint64_t CreateDevMenuStructureAddr = 0;
uint64_t AllocDevMenuMemoryforStructureAddr = 0;
uint64_t DevMenuCreateHeaderAddr = 0;
uint64_t DevMenuCreateEntryAddr = 0;
uint64_t DevMenuAddBoolAddr = 0;
uint64_t DevMenuAddFuncButtonAddr = 0;
uint64_t DevMenuAddIntSliderAddr = 0;
uint64_t MeleeMenuHook_ReturnAddr = 0;
uint64_t EntitySpawner_SpawnAddr = 0;
uint64_t EntitySpawnerAddr = 0;
uint64_t LoadLevelByNameAddr = 0;
uint64_t LoadActorByNameAddr = 0;

bool test_boolean[10] = { false };
int32_t test_int = 100;
int32_t test_int2 = 10;
char menu_entry_text[128];
char BuildVer[128];

const char* AppendEllipsisToText(const char* text)
{
    SecureZeroMemory((void*)menu_entry_text, sizeof(menu_entry_text));
    _snprintf_s(menu_entry_text, sizeof(menu_entry_text), "%s...", text);
    return menu_entry_text;
}

const char* FormatEntryText(const char* fmt, ...)
{
    SecureZeroMemory((void*)menu_entry_text, sizeof(menu_entry_text));
    va_list args;
    va_start(args, fmt);
    vsprintf_s(menu_entry_text, sizeof(menu_entry_text), fmt, args);
    va_end(args);
    return menu_entry_text;
}

int32_t CrashTest_OnClick(DMenu_ClickStructure DMenu, int32_t click_mode);
int32_t SetPlayerHealth_OnClick(DMenu_ClickStructure DMenu, int32_t click_mode);
int32_t SpawnTest_OnClick(DMenu_ClickStructure DMenu, int32_t click_mode);

void MakeMeleeMenu(uintptr_t menu_structure)
{
    FUNCTION_PTR(uintptr_t, CreateDevMenuStructure_Caller, CreateDevMenuStructureAddr, uintptr_t menu_structure, uintptr_t last_menu_structure);
    FUNCTION_PTR(uintptr_t, AllocDevMenuMemoryforStructure_Caller, AllocDevMenuMemoryforStructureAddr, uint32_t menu_size, uint32_t alignment, const char* source_func, uint32_t source_line, const char* source_file);
    FUNCTION_PTR(uintptr_t, DevMenuCreateHeader_Caller, DevMenuCreateHeaderAddr, uintptr_t menu_structure_ptr, const char* title, uint32_t unk);
    FUNCTION_PTR(uintptr_t, DevMenuAddBool_Caller, DevMenuAddBoolAddr, uintptr_t menu_structure_ptr, const char* bool_name, bool* bool_var, const char* bool_description);
    FUNCTION_PTR(uintptr_t, DevMenuAddFuncButton_Caller, DevMenuAddFuncButtonAddr, uintptr_t menu_structure_ptr, const char* func_name, void* func_target, uint32_t arg, bool* unk_bool);
    FUNCTION_PTR(uintptr_t, DevMenuAddIntSlider_Caller, DevMenuAddIntSliderAddr, uintptr_t menu_structure_ptr, const char* int_name, int32_t * int_val, int32_t * step_val, DMenu_Int args, const char* int_description);
    FUNCTION_PTR(uintptr_t, DevMenuCreateEntry_Caller, DevMenuCreateEntryAddr, uintptr_t menu_structure_ptr, const char* name, uintptr_t last_menu_structure_ptr, uint32_t unk, uint32_t unk2, const char* entry_description);
    // Create the header
    uint32_t header_menu_size = 224;
    uintptr_t Header_ptr = AllocDevMenuMemoryforStructure_Caller(header_menu_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    uintptr_t SubHeaderPtr = 0;
    const char* MyMenuEntryText = "Example Menu";
    if (Header_ptr)
    {
        SecureZeroMemory((void*)Header_ptr, header_menu_size);
        SubHeaderPtr = DevMenuCreateHeader_Caller(Header_ptr, MyMenuEntryText, 0);
    }
    uint32_t int_button_size = 368;
    uintptr_t IntButton_ptr = AllocDevMenuMemoryforStructure_Caller(int_button_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    uintptr_t IntButton_structure = 0;
    if (IntButton_ptr)
    {
        DMenu_Int int_args{};
        int_args.step_size = 10;
        int_args.min_val = test_int2;
        int_args.max_val = 1000;
        int_args.unk = 0;
        SecureZeroMemory((void*)IntButton_ptr, int_button_size);
        IntButton_structure = DevMenuAddIntSlider_Caller(IntButton_ptr, "Set fill ammo amount", &test_int2, &int_args.min_val, int_args, "For use with fill ammo");
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, IntButton_structure);
    uint32_t func_button_size = 200;
    uintptr_t FuncButton_ptr = AllocDevMenuMemoryforStructure_Caller(func_button_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    uintptr_t funcButton_structure = 0;
    if (FuncButton_ptr)
    {
        SecureZeroMemory((void*)FuncButton_ptr, func_button_size);
        funcButton_structure = DevMenuAddFuncButton_Caller(FuncButton_ptr, "Fill ammo", (void*)CrashTest_OnClick, 0, 0);
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, funcButton_structure);
    int_button_size = 368;
    IntButton_ptr = AllocDevMenuMemoryforStructure_Caller(int_button_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    IntButton_structure = 0;
    if (IntButton_ptr)
    {
        DMenu_Int int_args{};
        int_args.step_size = 10;
        int_args.min_val = 10;
        int_args.max_val = 100;
        int_args.unk = 0;
        SecureZeroMemory((void*)IntButton_ptr, int_button_size);
        IntButton_structure = DevMenuAddIntSlider_Caller(IntButton_ptr, "Set health amount", &test_int, &int_args.min_val, int_args, "For use with set player health");
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, IntButton_structure);
    func_button_size = 200;
    FuncButton_ptr = AllocDevMenuMemoryforStructure_Caller(func_button_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    funcButton_structure = 0;
    if (FuncButton_ptr)
    {
        SecureZeroMemory((void*)FuncButton_ptr, func_button_size);
        funcButton_structure = DevMenuAddFuncButton_Caller(FuncButton_ptr, "Set player health", (void*)SetPlayerHealth_OnClick, 0, 0);
    }
    CreateDevMenuStructure_Caller(SubHeaderPtr, funcButton_structure);

    func_button_size = 200;
    FuncButton_ptr = AllocDevMenuMemoryforStructure_Caller(func_button_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    funcButton_structure = 0;
    if (FuncButton_ptr)
    {
        SecureZeroMemory((void*)FuncButton_ptr, func_button_size);
        funcButton_structure = DevMenuAddFuncButton_Caller(FuncButton_ptr, "Set player health", (void*)SpawnTest_OnClick, 0, 0);
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
            SecureZeroMemory((void*)FirstBool_ptr, bool_menu_size);
            BoolPtr = DevMenuAddBool_Caller(FirstBool_ptr, FormatEntryText("Test Bool %u", i + 1), &test_boolean[i], nullptr);
        }
        CreateDevMenuStructure_Caller(SubHeaderPtr, BoolPtr);
    }
    // Create the entry point
    uint32_t entry_menu_size = 200;
    uintptr_t entry_ptr = AllocDevMenuMemoryforStructure_Caller(entry_menu_size, 16, __FUNCSIG__, __LINE__, __FILE__);
    if (entry_ptr)
    {
        SecureZeroMemory((void*)entry_ptr, entry_menu_size);
        DevMenuCreateEntry_Caller(entry_ptr, AppendEllipsisToText(MyMenuEntryText), Header_ptr, 0, 0, BuildVer);
    }
    CreateDevMenuStructure_Caller(menu_structure, entry_ptr);
    memset(menu_entry_text, 0, sizeof(menu_entry_text));
}

int32_t SpawnTest_OnClick(DMenu_ClickStructure DMenu, int32_t click_mode)
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
        const char* get_spawner_native_name = "get-spawner";
        StringId64 get_spawner_hash = SID(get_spawner_native_name);
        uintptr_t get_spawner_LookupPtr = ScriptManager_LookupClass(get_spawner_hash, 0);
        if (get_spawner_LookupPtr &&
            EntitySpawnerAddr &&
            LoadLevelByNameAddr &&
            LoadActorByNameAddr)
        {
            get_spawner_LookupPtr = *reinterpret_cast<uintptr_t*>(get_spawner_LookupPtr + 8);
            printf_s("#%.16llx (%s) found at 0x%016llx\n", get_spawner_hash, get_spawner_native_name, get_spawner_LookupPtr);
            const char* spawner_name = "npc-lab-lower-1";
            struct spawner_struct
            {
                float Spawner_x;
                float Spawner_y;
                float Spawner_z;
                uint8_t unk[0x75];
                uint8_t spawner_multi;
            };
            StringId64 spawner_hash = SID(spawner_name);
            FUNCTION_PTR(void, get_spawner_caller, get_spawner_LookupPtr, spawner_struct**, uint32_t argc, StringId64 * spawnerID);
            uint32_t argc = 1;
            printf_s("Firing #%.16llx (%s) with %u arguments\n", get_spawner_hash, get_spawner_native_name, argc);
            printf_s("Arg: #%.16llx (%s)\n", spawner_hash, spawner_name);
            spawner_struct* spawner_ptr{};
            get_spawner_caller(&spawner_ptr, argc, &spawner_hash);
            printf_s("#%.16llx (%s) returned 0x%016llx\n", get_spawner_hash, get_spawner_native_name, spawner_ptr);
            struct player_cord_struct
            {
                float world_x;
                float world_y;
                float world_z;
            };
            float player_cord[3] = { 0 };
            if (PlayerPtrAddr)
            {
                struct player_cord_struct* pointer;
                void* addr = (void*)(PlayerPtrAddr + 0x1b0);
                pointer = (struct player_cord_struct*)addr;
                player_cord[0] = pointer->world_x;
                player_cord[1] = pointer->world_y;
                player_cord[2] = pointer->world_z;
                printf_s("Player %.3f %.3f %.3f\n", player_cord[0], player_cord[1], player_cord[2]);
            }
            else
            {
                printf_s("not found\n");
                return 0;
            }
            // FUNCTION_PTR(void, LoadActorByName_Caller, LoadActorByNameAddr, const char* actor_name, uint32_t arg1, uint32_t arg2);
            FUNCTION_PTR(void, LoadLevelByName_Caller, LoadLevelByNameAddr, const char* level_name, uint64_t arg1);
            LoadLevelByName_Caller("lab-const", 0);
            LoadLevelByName_Caller("lab-lower-floor-ai", 0);
            if (spawner_ptr && PlayerPtrAddr)
            {
                float before_spawn_x = spawner_ptr->Spawner_x;
                float before_spawn_y = spawner_ptr->Spawner_y;
                float before_spawn_z = spawner_ptr->Spawner_z;
                uint8_t before_spawner_flag = spawner_ptr->spawner_multi;
                spawner_ptr->Spawner_x = player_cord[0];
                spawner_ptr->Spawner_y = player_cord[1];
                spawner_ptr->Spawner_z = player_cord[2];
                spawner_ptr->spawner_multi = 0x10;
                printf_s("Before %.3f %.3f %.3f 0x%02x\n", before_spawn_x, before_spawn_y, before_spawn_z, before_spawner_flag);
                printf_s("After %.3f %.3f %.3f 0x%02x\n", spawner_ptr->Spawner_x, spawner_ptr->Spawner_y, spawner_ptr->Spawner_z, spawner_ptr->spawner_multi);
                FUNCTION_PTR(void, EntitySpawner_Caller, EntitySpawnerAddr, spawner_struct*, uint32_t arg1, uint64_t arg2, uint32_t arg3, uint32_t arg4);
                EntitySpawner_Caller(spawner_ptr, 0, 0, 0, 0);
                spawner_ptr->Spawner_x = before_spawn_x;
                spawner_ptr->Spawner_y = before_spawn_y;
                spawner_ptr->Spawner_z = before_spawn_z;
                spawner_ptr->spawner_multi = before_spawner_flag;
                return 1;
            }
        }
        else
        {
            printf_s("Can't find #%.16llx (%s)!\n", get_spawner_hash, get_spawner_native_name);
        }
    }
    return 0;
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
            FUNCTION_PTR(void, NativeFunc, LookupPtr, uint64_t * ret_val, uint32_t argc, uint32_t * argv_first);
            uint32_t health_value = test_int;
            uint32_t argc = 1;
            uint64_t ret_func = 0;
            printf_s("Firing #%.16llx (%s) with %u arguments\nArg: %u\n", native_hash, native_name, argc, health_value);
            NativeFunc(&ret_func, argc, &health_value);
            printf_s("#%.16llx (%s) returned 0x%016llx\n", native_hash, native_name, ret_func);
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
        const char* native_name = "spawn-player";
        StringId64 native_hash = SID(native_name);
        uintptr_t LookupPtr = ScriptManager_LookupClass(native_hash, 1);
        if (LookupPtr && PlayerPtrAddr)
        {
            LookupPtr = *reinterpret_cast<uintptr_t*>(LookupPtr + 8);
            printf_s("#%.16llx (%s) found at 0x%016llx\n", native_hash, native_name, LookupPtr);
            FUNCTION_PTR(void, NativeFunc, LookupPtr, uint64_t * ret_val, uint32_t argc, uint64_t * argv_first, uint32_t * argv_sec);
            uint64_t ammo_value = 0;
            uint32_t ammo_value2 = 0;
            uint32_t argc = 2;
            uint64_t ret_func = 0;
            printf_s("Firing #%.16llx (%s) with %u arguments\nArg: %u\n", native_hash, native_name, argc, ammo_value);
            NativeFunc(&ret_func, argc, &ammo_value, &ammo_value2);
            printf_s("#%.16llx (%s) returned 0x%016llx\n", native_hash, native_name, ret_func);
            return 1;
        }
        else
        {
            printf_s("Can't find #%.16llx (%s)!\n", native_hash, native_name);
        }
    }
    return 0;
}
