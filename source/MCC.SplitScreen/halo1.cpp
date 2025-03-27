#include "helper.hpp"
#include "memory.hpp"

void PatchHalo1(HMODULE Module)
{
    if (Module)
    {
        const uintptr_t FreezeFix = FindAndPrintPatternW(Module, L"c6 05 ? ? ? ? 00 85 d2 0f 84 0a 01 00 00", __FUNCTIONW__ "_FreezeFix", 6);
        if (FreezeFix)
        {
            static const uint8_t one = 1;
            Memory::PatchBytes(FreezeFix, &one, sizeof(one));
        }
    }
}
