#pragma once

#include <stdint.h>
#include "memory.hpp"

struct FLAG_CAT;
extern FLAG_CAT* g_Cat;

// https://github.com/emoose/NieRAutomata-LodMod/blob/fc7316bd3309ab38eb2a4016a96f9282d285fb90/Automata-LodMod/SDKEnums.h

// Flags are worked on in each uint32_t 'chunk'
// (as can be seen at eg. NieRAutomata.exe+0x382570)
static constexpr uint32_t GetFlagOffset(uint32_t flagIndex)
{
    return (flagIndex / 32) * 4;
}

static constexpr uint32_t GetRawFlag(uint32_t flagIndex)
{
    return 0x80000000 >> (flagIndex & 0x1F);
}

static constexpr uint32_t GetFlagValue(uint32_t flagIndex, uint32_t& offset)
{
    offset = GetFlagOffset(flagIndex);
    return GetRawFlag(flagIndex);
}

static uint32_t* FlagPtr(const uint32_t cat, uint32_t flag, uint32_t& rawFlag)
{
    uint32_t offset = 0;
    rawFlag = GetFlagValue(flag, offset);
    return GameAddress<uint32_t*>((uintptr_t)g_Cat[cat].cat_list + offset);
}

static uint32_t* FlagPtr(const uint32_t cat, uint32_t flag)
{
    uint32_t offset = 0;
    GetFlagValue(flag, offset);
    return GameAddress<uint32_t*>((uintptr_t)g_Cat[cat].cat_list + offset);
}

static int CheckFlag(const uint32_t cat, uint32_t flag)
{
    uint32_t rawFlag = 0;
    uint32_t* ptr = FlagPtr(cat, flag, rawFlag);
    return ((*ptr & rawFlag) != 0);
}

static int CheckFlag(const uint32_t cat, uint32_t flag, uint32_t** retptr)
{
    uint32_t rawFlag = 0;
    uint32_t* ptr = FlagPtr(cat, flag, rawFlag);
    *retptr = ptr;
    return ((*ptr & rawFlag) != 0);
}

static void SetFlag(const uint32_t cat, uint32_t flag, const char* flag_name = 0)
{
    uint32_t rawFlag = 0;
    uint32_t* ptr = FlagPtr(cat, flag, rawFlag);
    *ptr ^= rawFlag;
    if (flag_name)
    {
        printf_s("`%s` (`0x%p`) set to `0x%x` (`0x%x`)\n", flag_name, ptr, *ptr, rawFlag);
    }
}
