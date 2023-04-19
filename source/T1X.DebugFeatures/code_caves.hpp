#include "helper.hpp"

#define MEM_TYPE 0x25 // TODO: Check this heap type

extern uint64_t Memory_PushAllocatorReturnAddr;
extern uint64_t GivePlayerWeapon_MainReturn;
extern uint64_t GivePlayerWeapon_SubReturn;
extern uint64_t GivePlayerWeapon_EntryReturn;
extern uint64_t Game_SnprintfAddr;
extern uint64_t GamePrintf;

void __attribute__((naked)) Memory_PushAllocator_CC();
void __attribute__((naked)) GivePlayerWeapon_MainCC();
void __attribute__((naked)) GivePlayerWeapon_SubCC();
void __attribute__((naked)) GivePlayerWeapon_EntryCC();
int ScriptPrintWarn_CC(char* buffer, char* fmt, ...);
