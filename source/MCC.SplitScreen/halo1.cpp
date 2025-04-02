#include "helper.hpp"
#include "memory.hpp"
#include "function_ptr.h"
#include "config.hpp"

extern bool bWantHalo1Fix;

extern HMODULE baseModule;

uiTYPEDEF_FUNCTION_PTR(void, RequestLevelLoad_Original, const char* param_1, uint32_t param_2);

static void RequestLevelLoadHook(const char* param_1, uint32_t param_2 /*likely is main menu */)
{
    bWantHalo1Fix = true;
    RequestLevelLoad_Original.ptr(param_1, param_2);
}

static void PatchFreezeFix(HMODULE Module);

void PatchHalo1(HMODULE Module)
{
    if (Module)
    {
        if (ConfigSettings.bHalo1FreezeFix)
        {
            PatchFreezeFix(Module);
        }
    }
}

static void PatchFreezeFix(HMODULE Module)
{
    // Patch from WinterSquire's AlphaRing module
    {
        const uintptr_t FreezeFix = FindAndPrintPatternW(Module, L"c6 05 ? ? ? ? 00 85 d2 0f 84 0a 01 00 00", __FUNCTIONW__ "_FreezeFix", 6);
        if (FreezeFix)
        {
            static const uint8_t one = 1;
            Memory::PatchBytes(FreezeFix, &one, sizeof(one));
        }
    }
    const uintptr_t RequestLevelLoadAddr = FindAndPrintPatternW(Module, L"41 8b d7 48 8d 0d ? ? ? ? e8 ? ? ? ? 41 8b dd", L"RequestLevelLoad", 10);
    if (RequestLevelLoadAddr)
    {
        const uintptr_t int3 = FindInt3Jmp(Module);
        if (int3)
        {
            RequestLevelLoad_Original.addr = ReadLEA32(RequestLevelLoadAddr, L"RequestLevelLoad_Original", 0, 1, 5);
            MAKE32CALL(RequestLevelLoadAddr, int3, RequestLevelLoadHook, 5);
        }
    }
}
