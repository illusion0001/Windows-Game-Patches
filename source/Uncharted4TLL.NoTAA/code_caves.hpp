#pragma once

#include "helper.hpp"

namespace CodeCave
{
    extern uintptr_t DMenu_Addr;
    extern uintptr_t DebugDraw_UpdateLoopAddr;
    extern uintptr_t DebugDraw_UpdateLoopReturnAddr;
    extern uintptr_t DMenu_Update;
    void __attribute__((naked)) DebugDraw_UpdateLoop();
};
