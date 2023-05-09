#include "code_caves.hpp"

extern uint64_t MeleeMenuHook_ReturnAddr;
extern uint64_t ReadCurrentLookIDAddr;
extern uint64_t InitProfileMenuAddr;
extern char BuildVer[128];

uintptr_t* ReadLookID_Hook(uintptr_t PlayerPtr, uintptr_t* LookIDPtr);

class DMenu
{
public:
	enum Message
	{
		None = 0,
		Unk1,
		Unk2,
		Unk3,
		Unk4,
		OnExecute
	};

	enum FunctionReturnCode
	{
		NoAction = 0,
		Failure = 0,
		Success = 1
	};

	struct OnExecuteStructure // 200 bytes
	{
		uintptr_t unk_array1[6];
		const char* DMENU_TEXT; // 0x30
		uintptr_t unk_array2[5];
		uint64_t DMENU_ARG; // 0x60
		uintptr_t unk_array3[8];
		uintptr_t DMENU_EXTRA_ARG; // 0xc8
		uintptr_t DMENU_FUNC; // 0xb0
	};

	struct IntSettings
	{
		int32_t step_size;
		int32_t min_val;
		int32_t max_val;
		int32_t unk;
	};
};

struct spawner_struct
{
	float Spawner_x;
	float Spawner_y;
	float Spawner_z;
	uint8_t unk[0x20];
	const char* spawner_name;
	uint8_t unk2[0x49];
	uint8_t spawner_multi;
};

TYPEDEF_FUNCTION_PTR(uintptr_t, ScriptManager_LookupClass, StringId64 sid, uint32_t unk);
TYPEDEF_FUNCTION_PTR(void, LoadLevelByName_Caller, const char* level_name, uint64_t arg1);
TYPEDEF_FUNCTION_PTR(void, EntitySpawner_Caller, spawner_struct* arg1, uint64_t* arg2, uint64_t* arg3, bool arg4, uint64_t* arg5);
TYPEDEF_FUNCTION_PTR(bool, LoadActorByName_Caller, const char* level_name, uint32_t arg1, bool sync_actor);

// DMenu
TYPEDEF_FUNCTION_PTR(uintptr_t, AllocDevMenuMemoryforStructure_Caller, uint32_t menu_size, uint32_t alignment, const char* source_func, uint32_t source_line, const char* source_file);
TYPEDEF_FUNCTION_PTR(uintptr_t, DevMenuCreateHeader_Caller, uintptr_t menu_structure_ptr, const char* title, uint32_t unk);
TYPEDEF_FUNCTION_PTR(uintptr_t, DevMenuAddBool_Caller, uintptr_t menu_structure_ptr, const char* bool_name, bool* bool_var, const char* bool_description);
TYPEDEF_FUNCTION_PTR(uintptr_t, DevMenuAddFuncButton_Caller, uintptr_t menu_structure_ptr, const char* func_name, void* ptrarget, uint64_t arg, bool* unk_bool);
TYPEDEF_FUNCTION_PTR(uintptr_t, DevMenuCreateEntry_Caller, uintptr_t menu_structure_ptr, const char* name, uintptr_t last_menu_structure_ptr, uint32_t unk, uint32_t unk2, const char* entry_description);
TYPEDEF_FUNCTION_PTR(uintptr_t, DevMenuAddIntSlider_Caller, uintptr_t menu_structure_ptr, const char* int_name, int32_t* int_val, int32_t* step_val, DMenu::IntSettings args, const char* int_description);
TYPEDEF_FUNCTION_PTR(uintptr_t, CreateDevMenuStructure_Caller, uintptr_t menu_structure, uintptr_t last_menu_structure);
