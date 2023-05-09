#include "dmenu.hpp"
#include "LookId.hpp"

INIT_FUNCTION_PTR(ScriptManager_LookupClass);
INIT_FUNCTION_PTR(LoadLevelByName_Caller);
INIT_FUNCTION_PTR(EntitySpawner_Caller);
INIT_FUNCTION_PTR(LoadActorByName_Caller);

INIT_FUNCTION_PTR(AllocDevMenuMemoryforStructure_Caller);
INIT_FUNCTION_PTR(DevMenuCreateHeader_Caller);
INIT_FUNCTION_PTR(DevMenuAddBool_Caller);
INIT_FUNCTION_PTR(DevMenuAddFuncButton_Caller);
INIT_FUNCTION_PTR(DevMenuCreateEntry_Caller);
INIT_FUNCTION_PTR(DevMenuAddIntSlider_Caller);
INIT_FUNCTION_PTR(CreateDevMenuStructure_Caller);

// player look id
uint64_t ReadCurrentLookIDAddr = 0;
bool OverrideLookID = false;
StringId64 CurrentOverrideLookID = 0;

bool test_boolean[10] = { false };
int32_t test_int = 100;
int32_t test_int2 = 10;
char menu_entry_text[128];
char BuildVer[128];

const char* AppendEllipsisToText(const char* text)
{
    CLEAR_MEM((void*)menu_entry_text, sizeof(menu_entry_text));
    _snprintf_s(menu_entry_text, sizeof(menu_entry_text), "%s...", text);
    return menu_entry_text;
}

const char* FormatEntryText(const char* fmt, ...)
{
    CLEAR_MEM((void*)menu_entry_text, sizeof(menu_entry_text));
    va_list args;
    va_start(args, fmt);
    vsprintf_s(menu_entry_text, sizeof(menu_entry_text), fmt, args);
    va_end(args);
    printf_s("%s\n", menu_entry_text);
    return menu_entry_text;
}

uint32_t spawn_count = 0;

uintptr_t* ReadLookID_Hook(uintptr_t PlayerPtr, uintptr_t* LookIDPtr)
{
    if (OverrideLookID)
    {
        *LookIDPtr = CurrentOverrideLookID;
        return LookIDPtr;
    }
    else
    {
        *LookIDPtr = *(uintptr_t*)(PlayerPtr + 0x1ce0);
        return LookIDPtr;
    }
}

bool SpawnTest_OnClick(DMenu::OnExecuteStructure DMenu, enum DMenu::Message Message)
{
    if (Message == DMenu::Message::OnExecute)
    {
        DMenu.DMENU_ARG = uint64_t(test_int);
        printf_s(
            str(DMenu::Message)": %i\n"
            str(&DMenu)": 0x%p\n"
            str(DMenu.DMENU_TEXT)": %s\n"
            str(DMenu.DMENU_ARG)": 0x%016llx\n"
            str(DMenu.DMENU_FUNC)": 0x%016llx\n",
            Message, &DMenu, DMenu.DMENU_TEXT, DMenu.DMENU_ARG, DMenu.DMENU_FUNC);
        const char* get_spawner_native_name = "get-spawner";
        StringId64 get_spawner_hash = SID(get_spawner_native_name);
        uintptr_t get_spawner_LookupPtr = ScriptManager_LookupClass(get_spawner_hash, 0);
        if (get_spawner_LookupPtr)
        {
            get_spawner_LookupPtr = *(uintptr_t*)(get_spawner_LookupPtr + 8);
            printf_s("#%.16llx (%s) found at 0x%016llx\n", get_spawner_hash, get_spawner_native_name, get_spawner_LookupPtr);
            const char* spawner_name = "npc-lab-lower-1";
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
                printf_s(str(PlayerPtrAddr) " not found\n");
                return DMenu::FunctionReturnCode::Failure;
            }
            LoadLevelByName_Caller("lab-lower-floor-ai", 0);
            if (spawner_ptr && PlayerPtrAddr)
            {
                float before_spawn_x = spawner_ptr->Spawner_x;
                float before_spawn_y = spawner_ptr->Spawner_y;
                float before_spawn_z = spawner_ptr->Spawner_z;
                const char* old_spawner = spawner_ptr->spawner_name;
                uint8_t before_spawner_flag = spawner_ptr->spawner_multi;
                char new_spawner[16] = { 0 };
                _snprintf_s(new_spawner, sizeof(new_spawner), "npc-%u", spawn_count);
                spawner_ptr->Spawner_x = player_cord[0];
                spawner_ptr->Spawner_y = player_cord[1];
                spawner_ptr->Spawner_z = player_cord[2];
                spawner_ptr->spawner_name = new_spawner;
                spawner_ptr->spawner_multi = 0x10;
                printf_s("Before %.3f %.3f %.3f 0x%02x %s\n", before_spawn_x, before_spawn_y, before_spawn_z, before_spawner_flag, old_spawner);
                printf_s("After %.3f %.3f %.3f 0x%02x %s\n", spawner_ptr->Spawner_x, spawner_ptr->Spawner_y, spawner_ptr->Spawner_z, spawner_ptr->spawner_multi, spawner_ptr->spawner_name);
                EntitySpawner_Caller(spawner_ptr, nullptr, nullptr, 1, nullptr);
                spawner_ptr->Spawner_x = before_spawn_x;
                spawner_ptr->Spawner_y = before_spawn_y;
                spawner_ptr->Spawner_z = before_spawn_z;
                spawner_ptr->spawner_name = old_spawner;
                spawner_ptr->spawner_multi = before_spawner_flag;
                spawn_count++;
                return DMenu::FunctionReturnCode::Success;
            }
        }
        else
        {
            printf_s("Can't find #%.16llx (%s)!\n", get_spawner_hash, get_spawner_native_name);
        }
    }
    return DMenu::FunctionReturnCode::NoAction;
}

bool SetPlayerHealth_OnClick(DMenu::OnExecuteStructure DMenu, enum DMenu::Message Message)
{
    if (Message == DMenu::Message::OnExecute)
    {
        DMenu.DMENU_ARG = uint64_t(test_int);
        printf_s(
            str(DMenu::Message)": %i\n"
            str(&DMenu)": 0x%p\n"
            str(DMenu.DMENU_TEXT)": %s\n"
            str(DMenu.DMENU_ARG)": 0x%016llx\n"
            str(DMenu.DMENU_FUNC)": 0x%016llx\n",
            Message, &DMenu, DMenu.DMENU_TEXT, DMenu.DMENU_ARG, DMenu.DMENU_FUNC);
        const char* native_name = "set-player-health";
        StringId64 native_hash = SID(native_name);
        uintptr_t LookupPtr = ScriptManager_LookupClass(native_hash, 1);
        if (LookupPtr)
        {
            LookupPtr = *(uintptr_t*)(LookupPtr + 8);
            printf_s("#%.16llx (%s) found at 0x%016llx\n", native_hash, native_name, LookupPtr);
            FUNCTION_PTR(void, NativeFunc, LookupPtr, uint64_t * ret_val, uint32_t argc, uint32_t * argv_first);
            uint32_t health_value = test_int;
            uint32_t argc = 1;
            uint64_t ret_func = 0;
            printf_s("Firing #%.16llx (%s) with %u arguments\nArg: %u\n", native_hash, native_name, argc, health_value);
            NativeFunc(&ret_func, argc, &health_value);
            printf_s("#%.16llx (%s) returned 0x%016llx\n", native_hash, native_name, ret_func);
            return DMenu::FunctionReturnCode::Success;
        }
        else
        {
            printf_s("Can't find #%.16llx (%s)!\n", native_hash, native_name);
            return DMenu::FunctionReturnCode::Failure;
        }
    }
    return DMenu::FunctionReturnCode::NoAction;
}

bool SpawnPlayer_OnClick(DMenu::OnExecuteStructure DMenu, enum DMenu::Message Message)
{
    if (Message == DMenu::Message::OnExecute)
    {
        CurrentOverrideLookID = DMenu.DMENU_ARG;
        printf_s(
            str(DMenu::Message)": %i\n"
            str(&DMenu)": 0x%p\n"
            str(DMenu.DMENU_TEXT)": %s\n"
            str(DMenu.DMENU_ARG)": 0x%016llx\n"
            str(DMenu.DMENU_FUNC)": 0x%016llx\n",
            Message, &DMenu, DMenu.DMENU_TEXT, DMenu.DMENU_ARG, DMenu.DMENU_FUNC);
        const char* spawn_player_native_name = "spawn-player";
        StringId64 spawn_player_native_hash = SID(spawn_player_native_name);
        uintptr_t spawn_player_LookupPtr = ScriptManager_LookupClass(spawn_player_native_hash, 1);
        if (spawn_player_LookupPtr)
        {
            spawn_player_LookupPtr = *(uintptr_t*)(spawn_player_LookupPtr + 8);
            printf_s("#%.16llx (%s) found at 0x%016llx\n", spawn_player_native_hash, spawn_player_native_name, spawn_player_LookupPtr);
            FUNCTION_PTR(void, spawn_player_NativeFunc, spawn_player_LookupPtr, uint64_t * ret_val, uint32_t argc, uint64_t * argv_first, uint32_t * argv_sec);
            uint64_t arg1 = 0;
            uint32_t arg2 = 0;
            uint32_t argc = 2;
            uint64_t ret_func = 0;
            printf_s("Firing #%.16llx (%s) with %u arguments\nArg: %u\n", spawn_player_native_hash, spawn_player_native_name, argc, arg1);
            spawn_player_NativeFunc(&ret_func, argc, &arg1, &arg2);
            printf_s("#%.16llx (%s) returned 0x%016llx\n", spawn_player_native_hash, spawn_player_native_name, ret_func);
            return DMenu::FunctionReturnCode::Success;
        }
        else
        {
            printf_s("Can't find #%.16llx (%s)!\n", spawn_player_native_hash, spawn_player_native_name);
            return DMenu::FunctionReturnCode::Failure;
        }
    }
    return DMenu::FunctionReturnCode::NoAction;
}

bool OnExecuteShowEntityInfo(DMenu::OnExecuteStructure DMenu, enum DMenu::Message Message)
{
    if (Message == DMenu::Message::OnExecute)
    {
        DMenu.DMENU_ARG = uint64_t(test_int2);
        printf_s(
            str(DMenu::Message)": %i\n"
            str(&DMenu)": 0x%p\n"
            str(DMenu.DMENU_TEXT)": %s\n"
            str(DMenu.DMENU_ARG)": 0x%016llx\n"
            str(DMenu.DMENU_FUNC)": 0x%016llx\n",
            Message, &DMenu, DMenu.DMENU_TEXT, DMenu.DMENU_ARG, DMenu.DMENU_FUNC);
        const char* native_name = "entity-info->string";
        const char* spawner_name = "npc-lab-lower-1";
        StringId64 native_hash = SID(native_name);
        StringId64 spawner_hash = SID(spawner_name);
        uintptr_t LookupPtr = ScriptManager_LookupClass(native_hash, 1);
        if (LookupPtr)
        {
            LookupPtr = *(uintptr_t*)(LookupPtr + 8);
            printf_s("#%.16llx (%s) found at 0x%016llx\n", native_hash, native_name, LookupPtr);
            FUNCTION_PTR(void, NativeFunc, LookupPtr, uint64_t * ret_val, uint32_t argc, StringId64 * argv_first);
            uint32_t argc = 1;
            uint64_t ret_func = 0;
            printf_s("Firing #%.16llx (%s) with %u arguments\nArg: '%s\n", native_hash, native_name, argc, spawner_name);
            NativeFunc(&ret_func, argc, &spawner_hash);
            printf_s("#%.16llx (%s) returned 0x%016llx\n", native_hash, native_name, ret_func);
            return DMenu::FunctionReturnCode::Success;
        }
        else
        {
            printf_s("Can't find #%.16llx (%s)!\n", native_hash, native_name);
            return DMenu::FunctionReturnCode::Failure;
        }
    }
    return DMenu::FunctionReturnCode::NoAction;
}

bool OnExecuteSpawnNPC(DMenu::OnExecuteStructure DMenu, enum DMenu::Message Message)
{
    if (Message == DMenu::Message::OnExecute)
    {
        StringId64 get_spawner_hash = SID("get-spawner");
        uintptr_t get_spawner_LookupPtr = ScriptManager_LookupClass(get_spawner_hash, 0);
        if (get_spawner_LookupPtr)
        {
            get_spawner_LookupPtr = *(uintptr_t*)(get_spawner_LookupPtr + 8);
            StringId64 spawner_hash = DMenu.DMENU_ARG;
            FUNCTION_PTR(void, get_spawner_caller, get_spawner_LookupPtr, spawner_struct**, uint32_t argc, StringId64 * spawnerID);
            uint32_t argc = 1;
            spawner_struct* spawner_ptr{};
            get_spawner_caller(&spawner_ptr, argc, &spawner_hash);
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
            }
            else
            {
                return DMenu::FunctionReturnCode::Failure;
            }
            LoadLevelByName_Caller("lab-lower-floor-ai", 0);
            if (spawner_ptr && PlayerPtrAddr)
            {
                float before_spawn_x = spawner_ptr->Spawner_x;
                float before_spawn_y = spawner_ptr->Spawner_y;
                float before_spawn_z = spawner_ptr->Spawner_z;
                const char* old_spawner = spawner_ptr->spawner_name;
                uint8_t before_spawner_flag = spawner_ptr->spawner_multi;
                char new_spawner[16] = { 0 };
                _snprintf_s(new_spawner, sizeof(new_spawner), "npc-%u", spawn_count);
                spawner_ptr->Spawner_x = player_cord[0];
                spawner_ptr->Spawner_y = player_cord[1];
                spawner_ptr->Spawner_z = player_cord[2];
                spawner_ptr->spawner_name = new_spawner;
                spawner_ptr->spawner_multi = 0x10;
                EntitySpawner_Caller(spawner_ptr, nullptr, nullptr, 1, nullptr);
                spawner_ptr->Spawner_x = before_spawn_x;
                spawner_ptr->Spawner_y = before_spawn_y;
                spawner_ptr->Spawner_z = before_spawn_z;
                spawner_ptr->spawner_name = old_spawner;
                spawner_ptr->spawner_multi = before_spawner_flag;
                spawn_count++;
                return DMenu::FunctionReturnCode::Success;
            }
        }
    }
    return DMenu::FunctionReturnCode::NoAction;
}

const char* anim_actor_list[] = {
    "anim-bill-facial",
    "anim-david-facial",
    "anim-dog-mm",
    "anim-dog-reactions",
    "anim-horse",
    "anim-horse-melee",
    "anim-horse-mm",
    "anim-npc-alert-caps",
    "anim-npc-ambi-caps",
    "anim-npc-brute",
    "anim-npc-f-ambi-explore-caps",
    "anim-npc-m",
    "anim-npc-m-ambi",
    "anim-npc-m-cover-share",
    "anim-npc-m-crouch",
    "anim-npc-m-enemy-only",
    "anim-npc-m-gestures",
    "anim-npc-m-horse",
    "anim-npc-m-lg",
    "anim-npc-m-lg-enemy-only",
    "anim-npc-m-mm-taps",
    "anim-npc-m-p-enemy-only",
    "anim-npc-m-p-enemy-only",
    "anim-npc-m-shared",
    "anim-npc-m-taps",
    "anim-npc-m-taps-bus",
    "anim-npc-m-turret",
    "anim-npc-m-uneasy",
    "anim-npc-m-wade",
    "anim-npc-uneasy-caps",
    "anim-player-gun-ellie",
    "anim-player-gun-shared",
    "anim-player-movement-push-pull",
    "anim-tess-facial",
    "t1x-anim-bill",
    "t1x-anim-bill-gestures",
    "t1x-anim-david",
    "t1x-anim-david-boss",
    "t1x-anim-david-gestures",
    "t1x-anim-ellie",
    "t1x-anim-ellie-player",
    "t1x-anim-ellie-player-gun",
    "t1x-anim-ellie-player-scavenge",
    "t1x-anim-ellie-v-david-melee",
    "t1x-anim-ellie-v-inf-dlc-melee",
    "t1x-anim-ellie-v-inf-melee",
    "t1x-anim-ellie-v-thug-melee",
    "t1x-anim-inf-v-tess-melee",
    "t1x-anim-joel",
    "t1x-anim-joel-gestures",
    "t1x-anim-joel-melee"
    "t1x-anim-joel-relaxed",
    "t1x-anim-joel-relaxed-sprint",
    "t1x-anim-npc-bliz",
    "t1x-anim-npc-gestures",
    "t1x-anim-npc-m-caps",
    "t1x-anim-npc-melee",
    "t1x-anim-npc-normal",
    "t1x-anim-tess",
    "t1x-anim-tess-caps",
    "t1x-anim-tess-gestures",
    "t1x-anim-tess-ladder",
    "t1x-tess-anim-wade",
};

const char* anim_actor_list_infected[] = {
    "anim-bloater",
    "anim-clicker",
    "anim-inf-legless",
    "anim-inf-melee",
    "anim-inf-melee-stalker",
    "anim-inf-shared",
    "anim-melee-abby-inf-t2",
    "anim-melee-ellie-inf-t2",
    "anim-melee-inf-bloater-t2",
    "anim-melee-inf-clicker-t2",
    "anim-melee-inf-crawler-t2",
    "anim-melee-inf-runner-t2",
    "anim-melee-inf-shared-t2",
    "anim-melee-inf-stalker-t2",
    "anim-melee-player-shared-inf-t2",
    "anim-runner",
    "anim-runner-horde",
    "anim-runr-stlkr-shared",
    "anim-stalker",
    "gore-muscle-tendon",
    "t1x-anim-bloater",
    "t1x-anim-inf-melee",
    "t1x-anim-inf-melee-clicker-female",
    "t1x-anim-inf-melee-clicker-male",
    "t1x-anim-inf-melee-runner-female",
    "t1x-anim-inf-melee-runner-male",
    "t1x-anim-inf-melee-stream-runner-female",
    "t1x-anim-inf-melee-stream-runner-male",
    "t1x-anim-inf-shared",
    "t1x-anim-inf-vs-npc-melee",
};

bool sync_actor = false;

bool OnExecuteLoadActor(DMenu::OnExecuteStructure DMenu, enum DMenu::Message Message)
{
    if (Message == DMenu::Message::OnExecute)
    {
        if (DMenu.DMENU_ARG == 0)
        {
            for (uint32_t i = 0; i < STRING_SIZEOF(anim_actor_list); i++)
            {
                printf_s("Actor open: %s\nSync mode: %i\n", anim_actor_list[i], sync_actor);
                bool load_ret = LoadActorByName_Caller(anim_actor_list[i], 0, sync_actor);
                printf_s("LoadActor Returned: 0x%02x\n", load_ret);
            }
        }
        else
        {
            for (uint32_t i = 0; i < STRING_SIZEOF(anim_actor_list_infected); i++)
            {
                printf_s("Actor open: %s\nSync mode: %i\n", anim_actor_list_infected[i], sync_actor);
                bool load_ret = LoadActorByName_Caller(anim_actor_list_infected[i], 0, sync_actor);
                printf_s("LoadActor Returned: 0x%02x\n", load_ret);
            }
        }
        return DMenu::FunctionReturnCode::Success;
    }
    return DMenu::FunctionReturnCode::NoAction;
}

bool OnExecuteShowTaskData(DMenu::OnExecuteStructure DMenu, enum DMenu::Message Message)
{
    if (Message == DMenu::Message::OnExecute)
    {
        struct ActiveTaskData
        {
            void* unk_callback;
            size_t buffer_size;
            size_t formatted_size;
            char text_buffer[512];
        };
        struct ActiveTaskData* pointer;
        void* addr = (void*)(TaskDataStructure);
        pointer = (struct ActiveTaskData*)addr;
        if (pointer->text_buffer[0] != 0)
        {
            printf_s("Task callback: 0x%016llx\nBuffer size: %lli\nFormatted size: %lli\nFormatted String: %s\n\n", pointer->unk_callback, pointer->buffer_size, pointer->formatted_size, pointer->text_buffer);
            return DMenu::FunctionReturnCode::Success;
        }
        return DMenu::FunctionReturnCode::Failure;
    }
    return DMenu::FunctionReturnCode::NoAction;
}

const char* npc_list1[] = {
    "npc-lab-lower-1",
    "npc-lab-lower-2",
    "npc-lab-lower-3",
    "npc-lab-lower-4",
    "npc-lab-lower-5",
    "npc-lab-lower-6",
    "npc-lab-lower-7",
    "npc-lab-lower-8",
    "npc-lab-lower-9",
    "npc-lab-lower-10",
};

constexpr uint32_t num_npc_list = sizeof(npc_list1) / sizeof(npc_list1[0]);

constexpr uint32_t bool_menu_size = 192;
constexpr uint32_t entry_menu_size = 200;
constexpr uint32_t func_button_size = 200;
constexpr uint32_t header_menu_size = 224;
constexpr uint32_t int_button_size = 368;

class MenuItem {
public:
    MenuItem(const char* name) : name_(name) {}
    virtual ~MenuItem() {}

    void addChild(MenuItem* child) {
        children_.push_back(child);
    }

    const char* getName() const {
        return name_;
    }

    const std::vector<MenuItem*>& getChildren() const {
        return children_;
    }

private:
    const char* name_;
    std::vector<MenuItem*> children_;
};

class Function_Button : public MenuItem {
public:
    typedef void* FunctionPtr;

    Function_Button(const char* name, FunctionPtr func, uint64_t arg) : MenuItem(name), func_(func), arg_(arg) {}

    const uint64_t getFuncArg() const
    {
        return arg_;
    }

    const uintptr_t getFuncAddr() const
    {
        return (uintptr_t)(void*)func_;
    }

private:
    FunctionPtr func_;
    uint64_t arg_;
};

struct Player {
    const char* name;
    const char** lookid;
    size_t numLookid;
};

uintptr_t Create_DMenu_Header(const char* title, const char* source_func, uint32_t source_line, const char* source_file)
{
    uintptr_t Header_ptr = AllocDevMenuMemoryforStructure_Caller(header_menu_size, 16, source_func, source_line, source_file);
    uintptr_t SubHeaderPtr = 0;
    if (Header_ptr)
    {
        CLEAR_MEM((void*)Header_ptr, header_menu_size);
        SubHeaderPtr = DevMenuCreateHeader_Caller(Header_ptr, title, 0);
        return SubHeaderPtr;
    }
    return 0;
}

void Create_DMenu_BoolButton(uintptr_t menu_structure, const char* button_name, const char* button_description, bool* bool_button, const char* source_func, uint32_t source_line, const char* source_file)
{
    // Create the bool
    uintptr_t FirstBool_ptr = AllocDevMenuMemoryforStructure_Caller(header_menu_size, 16, source_func, source_line, source_file);
    uintptr_t BoolPtr = 0;
    if (FirstBool_ptr)
    {
        CLEAR_MEM((void*)FirstBool_ptr, bool_menu_size);
        BoolPtr = DevMenuAddBool_Caller(FirstBool_ptr, button_name, bool_button, button_description);
    }
    CreateDevMenuStructure_Caller(menu_structure, BoolPtr);
}

void Create_DMenu_FunctionButton(uintptr_t menu_structure, const char* button_name, void* function_button, uint64_t arg, const char* source_func, uint32_t source_line, const char* source_file)
{
    uintptr_t FuncButton_ptr = AllocDevMenuMemoryforStructure_Caller(func_button_size, 16, source_func, source_line, source_file);
    uintptr_t funcButton_structure = 0;
    if (FuncButton_ptr)
    {
        CLEAR_MEM((void*)FuncButton_ptr, func_button_size);
        funcButton_structure = DevMenuAddFuncButton_Caller(FuncButton_ptr, button_name, (void*)function_button, arg, 0);
    }
    CreateDevMenuStructure_Caller(menu_structure, funcButton_structure);
}

void Create_DMenu_IntSlider(uintptr_t menu_structure, const char* button_name, const char* button_description, int32_t* int_value, DMenu::IntSettings int_args, const char* source_func, uint32_t source_line, const char* source_file)
{
    uintptr_t IntButton_ptr = AllocDevMenuMemoryforStructure_Caller(int_button_size, 16, source_func, source_line, source_file);
    uintptr_t IntButton_structure = 0;
    if (IntButton_ptr)
    {
        CLEAR_MEM((void*)IntButton_ptr, int_button_size);
        IntButton_structure = DevMenuAddIntSlider_Caller(IntButton_ptr, button_name, int_value, &int_args.min_val, int_args, button_description);
    }
    CreateDevMenuStructure_Caller(menu_structure, IntButton_structure);
}

void Create_DMenu_Entry(uintptr_t menu_structure, uintptr_t SubHeaderPtr, const char* title, const char* description, const char* source_func, uint32_t source_line, const char* source_file)
{
    // Create the entry point
    uintptr_t entry_ptr = AllocDevMenuMemoryforStructure_Caller(entry_menu_size, 16, source_func, source_line, source_file);
    if (entry_ptr)
    {
        CLEAR_MEM((void*)entry_ptr, entry_menu_size);
        DevMenuCreateEntry_Caller(entry_ptr, AppendEllipsisToText(title), SubHeaderPtr, 0, 0, description);
    }
    CreateDevMenuStructure_Caller(menu_structure, entry_ptr);
}

void MakeMeleeMenu(uintptr_t menu_structure)
{
    uintptr_t Header_ptr = Create_DMenu_Header("Custom", __FUNCSIG__, __LINE__, __FILE__);
    const char* lab_level_name = "lab-lower-floor-ai";
    uintptr_t lab_level_header_ptr = Create_DMenu_Header(lab_level_name, __FUNCSIG__, __LINE__, __FILE__);

    for (uint32_t i = 0; i < num_npc_list; i++)
    {
        Create_DMenu_FunctionButton(lab_level_header_ptr, npc_list1[i], (void*)OnExecuteSpawnNPC, SID(npc_list1[i]), __FUNCSIG__, __LINE__, __FILE__);
    }
    Create_DMenu_Entry(Header_ptr, lab_level_header_ptr, lab_level_name, nullptr, __FUNCSIG__, __LINE__, __FILE__);

    Player Player_LookID[] = {
        {"Player Bill", LookID::bill_lookid, STRING_SIZEOF(LookID::bill_lookid)},
        {"Player David", LookID::david_lookid, STRING_SIZEOF(LookID::david_lookid)},
        {"Player Ellie", LookID::ellie_lookid, STRING_SIZEOF(LookID::ellie_lookid)},
        {"Player Henry", LookID::henry_lookid, STRING_SIZEOF(LookID::henry_lookid)},
        {"Player James", LookID::james_lookid, STRING_SIZEOF(LookID::james_lookid)},
        {"Player Joel", LookID::joel_lookid, STRING_SIZEOF(LookID::joel_lookid)},
        {"Player Maria", LookID::maria_lookid, STRING_SIZEOF(LookID::maria_lookid)},
        {"Player Marlene", LookID::marlene_lookid, STRING_SIZEOF(LookID::marlene_lookid)},
        {"Player Riley", LookID::riley_lookid, STRING_SIZEOF(LookID::riley_lookid)},
        {"Player Robert", LookID::robert_lookid, STRING_SIZEOF(LookID::robert_lookid)},
        {"Player Sam", LookID::sam_lookid, STRING_SIZEOF(LookID::sam_lookid)},
        {"Player Sarah", LookID::sarah0_lookid, STRING_SIZEOF(LookID::sarah0_lookid)},
        {"Player (Sarah)", LookID::sarah1_lookid, STRING_SIZEOF(LookID::sarah1_lookid)},
        {"Player Tess", LookID::tess_lookid, STRING_SIZEOF(LookID::tess_lookid)},
        {"Player Tommy", LookID::tommy_lookid, STRING_SIZEOF(LookID::tommy_lookid)},
        {"Player NPC - Cannibal", LookID::cannibal_lookid, STRING_SIZEOF(LookID::cannibal_lookid)},
        {"Player NPC - Hunter", LookID::hunter_lookid, STRING_SIZEOF(LookID::hunter_lookid)},
        {"Player NPC - Infected", LookID::infected_lookid, STRING_SIZEOF(LookID::infected_lookid)},
        {"Player NPC - Jackson", LookID::jackson_lookid, STRING_SIZEOF(LookID::jackson_lookid)},
        {"Player NPC - Marauder", LookID::marauder_lookid, STRING_SIZEOF(LookID::marauder_lookid)},
        {"Player NPC - Military", LookID::military_lookid, STRING_SIZEOF(LookID::military_lookid)},
        {"Player NPC - Smuggler", LookID::smuggler_lookid, STRING_SIZEOF(LookID::smuggler_lookid)},
        {"Player NPC - Survivor", LookID::survivor_lookid, STRING_SIZEOF(LookID::survivor_lookid)},
        {"Player NPC - Texan", LookID::texan_lookid, STRING_SIZEOF(LookID::texan_lookid)},
        {"Player NPC - Tourist", LookID::tourist_lookid, STRING_SIZEOF(LookID::tourist_lookid)},
        {"Player Misc", LookID::misc_lookid, STRING_SIZEOF(LookID::misc_lookid)},
        {"Player NPC", LookID::npc_lookid, STRING_SIZEOF(LookID::npc_lookid)},
        {"Player MP", LookID::mp_lookid, STRING_SIZEOF(LookID::mp_lookid)},
        {"Player Test", LookID::test_lookid, STRING_SIZEOF(LookID::test_lookid)},
        {"NPC", LookID::npc2_lookid, STRING_SIZEOF(LookID::npc2_lookid)},
        {"Militia 1Hmelee", LookID::militia_1, STRING_SIZEOF(LookID::militia_1)},
        {"Militia Baseball Bat", LookID::militia_2, STRING_SIZEOF(LookID::militia_2)},
        {"Militia Pistol", LookID::militia_3, STRING_SIZEOF(LookID::militia_3)},
        {"Militia Rifle Remington Bolt", LookID::militia_4, STRING_SIZEOF(LookID::militia_4)},
        {"Militia Rifle Semi Auto", LookID::militia_5, STRING_SIZEOF(LookID::militia_5)},
        {"Militia Shotgun", LookID::militia_6, STRING_SIZEOF(LookID::militia_6)},
        {"Militia Hybrid Pistol - 1Hmelee", LookID::militia_7, STRING_SIZEOF(LookID::militia_7)},
        {"Militia Hybrid Rifle - 1Hmelee", LookID::militia_8, STRING_SIZEOF(LookID::militia_8)},
        {"Militia Hybrid Pistol - 2Hmelee", LookID::militia_9, STRING_SIZEOF(LookID::militia_9)},
        {"Militia Hybrid Rifle - 2Hmelee", LookID::militia_10, STRING_SIZEOF(LookID::militia_10)},
        {"Infected Runner", LookID::runner_lookid, STRING_SIZEOF(LookID::runner_lookid)},
        {"Infected Stalker", LookID::stalker_lookid, STRING_SIZEOF(LookID::stalker_lookid)},
        {"Gore", LookID::gore_lookid, STRING_SIZEOF(LookID::gore_lookid)},
        {"TD NPC test spawns", LookID::mp_td_npc, STRING_SIZEOF(LookID::mp_td_npc)},
        {"Horses", LookID::horses_t1, STRING_SIZEOF(LookID::horses_t1)},
    };

    MenuItem root("DC Spawn");

    for (uint32_t i = 0; i < sizeof(Player_LookID) / sizeof(Player); i++) {
        Player& player = Player_LookID[i];
        MenuItem* submenu = new MenuItem(player.name);
        for (uint32_t j = 0; j < player.numLookid; j++) {
#if 0
            void* target_func = nullptr;
            const char player_str[] = "Player";
            if (strncmp(player_str, player.name, sizeof(player_str) - 1) == 0)
                target_func = (void*)SpawnPlayer_OnClick;
            else
                target_func = (void*)SpawnTest_OnClick;
#endif
            const char* costume = player.lookid[j];
            submenu->addChild(new Function_Button(costume, (void*)SpawnPlayer_OnClick /* target_func */, SID(costume)));
        }
        root.addChild(submenu);
    }

    uintptr_t SchemaHeader_ptr = Create_DMenu_Header(root.getName(), __FUNCSIG__, __LINE__, __FILE__);
    for (auto submenu : root.getChildren())
    {
        uintptr_t SubSchemaHeader_ptr = Create_DMenu_Header(submenu->getName(), __FUNCSIG__, __LINE__, __FILE__);
        for (auto button : submenu->getChildren())
        {
            if (dynamic_cast<Function_Button*>(button))
            {
                Function_Button* func_button = static_cast<Function_Button*>(button);
                Create_DMenu_FunctionButton(SubSchemaHeader_ptr, func_button->getName(), (void*)func_button->getFuncAddr(), func_button->getFuncArg(), __FUNCSIG__, __LINE__, __FILE__);
            }
        }
        Create_DMenu_Entry(SchemaHeader_ptr, SubSchemaHeader_ptr, submenu->getName(), nullptr, __FUNCSIG__, __LINE__, __FILE__);
    }
    Create_DMenu_Entry(Header_ptr, SchemaHeader_ptr, root.getName(), nullptr, __FUNCSIG__, __LINE__, __FILE__);

    Create_DMenu_FunctionButton(Header_ptr, "Respawn Player", (void*)SpawnPlayer_OnClick, 0, __FUNCSIG__, __LINE__, __FILE__);
    DMenu::IntSettings int_args{};
    int_args.step_size = 10;
    int_args.min_val = 10;
    int_args.max_val = 100;
    int_args.unk = 0;
    Create_DMenu_IntSlider(Header_ptr, "Set health amount", "For use with set player health", &test_int, int_args, __FUNCSIG__, __LINE__, __FILE__);
    Create_DMenu_FunctionButton(Header_ptr, "Set Player Health", (void*)SetPlayerHealth_OnClick, 0, __FUNCSIG__, __LINE__, __FILE__);
    Create_DMenu_FunctionButton(Header_ptr, "Spawn Test", (void*)SpawnTest_OnClick, 0, __FUNCSIG__, __LINE__, __FILE__);
    Create_DMenu_FunctionButton(Header_ptr, "Show Entity Info", (void*)OnExecuteShowEntityInfo, 0, __FUNCSIG__, __LINE__, __FILE__);
    Create_DMenu_BoolButton(Header_ptr, "Sync actor after load", nullptr, &sync_actor, __FUNCSIG__, __LINE__, __FILE__);
    Create_DMenu_FunctionButton(Header_ptr, "Try to load animations", (void*)OnExecuteLoadActor, 0, __FUNCSIG__, __LINE__, __FILE__);
    Create_DMenu_FunctionButton(Header_ptr, "Try to load infected animations data", (void*)OnExecuteLoadActor, 1, __FUNCSIG__, __LINE__, __FILE__);
    Create_DMenu_FunctionButton(Header_ptr, "Show task data", (void*)OnExecuteShowTaskData, 0, __FUNCSIG__, __LINE__, __FILE__);
    Create_DMenu_BoolButton(Header_ptr, "Experimental LookID Override", "Enable for DC Spawn", &OverrideLookID, __FUNCSIG__, __LINE__, __FILE__);
    Create_DMenu_Entry(menu_structure, Header_ptr, "Custom Menu", BuildVer, __FUNCSIG__, __LINE__, __FILE__);
}
