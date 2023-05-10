#include "helper.hpp"

#define MEM_TYPE 0x25 // "Script Data" Memory Context

#pragma once

#define FUNCTION_PTR(return_type, func_name, func_addr, ...) \
    typedef return_type (*func_name##_t)(__VA_ARGS__); \
    func_name##_t func_name = (func_name##_t)(func_addr);

#define TYPEDEF_FUNCTION_PTR(ret_type, func_name, ...) \
    typedef ret_type (*func_name##_ptr)(__VA_ARGS__); \
    extern func_name##_ptr func_name;

#define INIT_FUNCTION_PTR(func_name) \
    func_name##_ptr func_name = nullptr

#define STRING_SIZEOF(str) sizeof(str) / sizeof(const char*)
#define CLEAR_MEM(input, size_input) memset(input, 0, size_input)

extern uint64_t Memory_PushAllocatorReturnAddr;
extern uint64_t Memory_NewHandlerReturnAddr;
extern uint64_t GivePlayerWeapon_MainReturn;
extern uint64_t GivePlayerWeapon_SubReturn;
extern uint64_t GivePlayerWeapon_EntryReturn;
extern uint64_t GivePlayerWeapon_EntryHeaderReturn;
extern uint64_t PlayerPtrAddr;
extern uint64_t ActiveTaskDisplayAddr;
extern uint64_t ActiveTaskDisplayReturnAddr;
extern uint64_t DebugPrint_WindowContext;
extern uint64_t DebugPrint_ReturnAddr;
extern uint64_t DebugPrint_OriginalAddr;

extern bool GameVeris1050;

class TaskManager
{
public:
    struct ActiveTaskData
    {
        void* unk_callback;
        size_t buffer_size;
        size_t formatted_size;
        char text_buffer[128];
    };
};

extern struct TaskManager::ActiveTaskData TaskData;
extern bool TestTextPrintV;
extern bool ShowPlayerPos;

TYPEDEF_FUNCTION_PTR(void, TextPrintV, uintptr_t WindowContext, float font_x, float font_y, float font_scale_x, float font_scale_y, uint32_t color, const char* fmt, ...);

void __attribute__((naked)) Memory_PushAllocator_CC();
void __attribute__((naked)) Memory_NewHandler_CC();
void __attribute__((naked)) GivePlayerWeapon_MainCC();
void __attribute__((naked)) GivePlayerWeapon_SubCC();
void __attribute__((naked)) GivePlayerWeapon_EntryCC();
void __attribute__((naked)) GivePlayerWeapon_EntryHeaderCC();
void __attribute__((naked)) GetPlayerPtrAddr_CC();
void __attribute__((naked)) ActiveTaskDisplay_CC();
void __attribute__((naked)) DebugPrint_CC();
const char* ScriptPrintWarn_CC(const char* ret, const char* fmt, ...);
void MakeMeleeMenu(uintptr_t menu_structure);
