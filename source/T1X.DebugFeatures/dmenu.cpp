#include "dmenu.hpp"

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
        AllocDevMenu1_Caller(Header_ptr, 0, header_menu_size);
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
        funcButton_structure = DevMenuAddFuncButton_Caller(FuncButton_ptr, "Set player health", (void*)SetPlayerHealth_OnClick, 0, 0);
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
        const char* native_name = "add-player-weapon-reserve-ammo";
        StringId64 native_hash = SID(native_name);
        uintptr_t LookupPtr = ScriptManager_LookupClass(native_hash, 1);
        if (LookupPtr)
        {
            LookupPtr = *reinterpret_cast<uintptr_t*>(LookupPtr + 8);
            printf_s("#%.16llx (%s) found at 0x%016llx\n", native_hash, native_name, LookupPtr);
            FUNCTION_PTR(void, NativeFunc, LookupPtr, uint64_t * ret_val, uint32_t argc, uint32_t * argv_first);
            uint32_t ammo_value = test_int2;
            uint32_t argc = 1;
            uint64_t ret_func = 0;
            printf_s("Firing #%.16llx (%s) with %u arguments\nArg: %u\n", native_hash, native_name, argc, ammo_value);
            NativeFunc(&ret_func, argc, &ammo_value);
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
