#include "code_caves.hpp"

extern uint64_t AllocMemoryforStructureAddr;
extern uint64_t CreateDevMenuStructureAddr;
extern uint64_t AllocDevMenuMemoryforStructureAddr;
extern uint64_t AllocDevMenu1Addr;
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
extern char BuildVer[128];

struct DMenu_ClickStructure // 200 bytes
{
	uintptr_t unk_array1[6];
	char* DMENU_TEXT; // 0x30
	uintptr_t unk_array2[5];
	uint64_t DMENU_ARG; // 0x60
	uintptr_t unk_array3[8];
	uintptr_t DMENU_EXTRA_ARG; // 0xc8
	uintptr_t DMENU_FUNC; // 0xb0
};

struct DMenu_Int
{
	int32_t step_size;
	int32_t min_val;
	int32_t max_val;
	int32_t unk;
};
