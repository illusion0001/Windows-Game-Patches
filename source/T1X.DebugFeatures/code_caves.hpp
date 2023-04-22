#include "helper.hpp"

#define MEM_TYPE 0x25 // TODO: Check this heap type

#define FUNCTION_PTR(return_type, func_name, func_addr, ...) \
    typedef return_type (*func_name##_t)(__VA_ARGS__); \
    func_name##_t func_name = (func_name##_t)(func_addr);


extern uint64_t Memory_PushAllocatorReturnAddr;
extern uint64_t GivePlayerWeapon_MainReturn;
extern uint64_t GivePlayerWeapon_SubReturn;
extern uint64_t GivePlayerWeapon_EntryReturn;
extern uint64_t Game_SnprintfAddr;
extern uint64_t ScriptLookupAddr;
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
extern char BuildVer[128];

struct DMenu_ClickStructure // 200 bytes
{
	uintptr_t unk_array1[6];
	char* DMENU_TEXT; // 0x30
	uintptr_t unk_array2[5];
	uint64_t DMENU_ARG; // 0x60
	uintptr_t unk_array3[9];
	uintptr_t DMENU_FUNC; // 0xb0
};

struct DMenu_Int
{
	int32_t step_size;
	int32_t min_val;
	int32_t max_val;
	int32_t unk;
};

void __attribute__((naked)) Memory_PushAllocator_CC();
void __attribute__((naked)) GivePlayerWeapon_MainCC();
void __attribute__((naked)) GivePlayerWeapon_SubCC();
void __attribute__((naked)) GivePlayerWeapon_EntryCC();
const char* ScriptPrintWarn_CC(void* unused, char* fmt, ...);
void MakeMeleeMenu(uintptr_t menu_structure);
