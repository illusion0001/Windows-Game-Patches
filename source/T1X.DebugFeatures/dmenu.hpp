#include "code_caves.hpp"

extern uint64_t AllocMemoryforStructureAddr;
extern uint64_t CreateDevMenuStructureAddr;
extern uint64_t AllocDevMenuMemoryforStructureAddr;
extern uint64_t DevMenuCreateHeaderAddr;
extern uint64_t DevMenuCreateEntryAddr;
extern uint64_t DevMenuAddBoolAddr;
extern uint64_t DevMenuAddFuncButtonAddr;
extern uint64_t DevMenuAddIntSliderAddr;
extern uint64_t MeleeMenuHook_ReturnAddr;
extern uint64_t EntitySpawner_SpawnAddr;
extern uint64_t EntitySpawnerAddr;
extern uint64_t LoadLevelByNameAddr;
extern uint64_t LoadActorByNameAddr;
extern uint64_t ReadCurrentLookIDAddr;
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
