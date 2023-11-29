#include "code_caves.hpp"

namespace CodeCave
{
    uintptr_t DMenu_Addr = 0;
    uintptr_t DebugDraw_UpdateLoopAddr = 0;
    uintptr_t DebugDraw_UpdateLoopReturnAddr = 0;
    uintptr_t DMenu_Update = 0;

    void __attribute__((naked)) DebugDraw_UpdateLoop()
    {
        __asm
        {
            mov rcx, [DMenu_Addr];
            mov rcx, [rcx];
            call[DMenu_Update];
            jmp[DebugDraw_UpdateLoopReturnAddr];
        }
    }
}
