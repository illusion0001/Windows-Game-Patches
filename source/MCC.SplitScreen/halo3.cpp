#include "helper.hpp"
#include "memory.hpp"
#include "function_ptr.h"
#include "config.hpp"

#include "SharePatch.hpp"

uiTYPEDEF_FUNCTION_PTR(void, LoadLevelInit_, const void* pData);

// not used in odst module since no dynamic cameras
uint8_t* g_LevelTickRate = 0;

static void LoadLevelInit(const void* pData)
{
    LoadLevelInit_.ptr(pData);
}

static void LoadLevelInit_Menu(const void* pData)
{
    // Menu camera fix
    *g_LevelTickRate = 30;
    LOG(L"Updated user tickrate: %d\n", *g_LevelTickRate);
    LoadLevelInit(pData);
    // reset
    *g_LevelTickRate = 0;
}

static void PatchMainMenu(HMODULE Module, bool isOdst)
{
    const uintptr_t LevelInit_ReadTickRateUserDefinedAddr = FindAndPrintPatternW(Module, L"88 83 ? ? ? ? 8a 0d ? ? ? ?", __FUNCTIONW__ "_LevelInit_ReadTickRateUserDefined", 6);
    const uintptr_t LevelLoadMenuAddr = FindAndPrintPatternW(Module, L"e8 ? ? ? ? 33 f6 c6 07 03", __FUNCTIONW__ "_LoadLevelMenu", 0);
    const uintptr_t ForbiddenMenuType3Addr = FindAndPrintPatternW(Module, L"80 3f 03 0f 84 ? ? ? ? e8 ? ? ? ?", __FUNCTIONW__ "_ForbiddenMenuType3", 3);
    if (LevelInit_ReadTickRateUserDefinedAddr && LevelLoadMenuAddr && ForbiddenMenuType3Addr)
    {
        Memory::MakeNops(ForbiddenMenuType3Addr, 6);
        if (!isOdst)
        {
            g_LevelTickRate = (uint8_t*)ReadLEA32(LevelInit_ReadTickRateUserDefinedAddr, L"g_LevelTickRate", 0, 2, 6);
            LoadLevelInit_.addr = ReadLEA32(LevelLoadMenuAddr, L"LoadLevelInit_", 0, 1, 5);
            const uintptr_t int3 = FindInt3Jmp(Module);
            if (int3)
            {
                MAKE32CALL(LevelLoadMenuAddr, int3, LoadLevelInit_Menu, 5);
            }
        }
    }
}

static void PatchPauseH3(HMODULE Module)
{
    const uintptr_t HostExistForPauseAddr = FindAndPrintPatternW(L"4d 85 db 0f 84 ? ? ? ? 38 1d ? ? ? ? 0f 85 ? ? ? ?", __FUNCTIONW__ "_HostExistForPause", 3);
    if (HostExistForPauseAddr)
    {
        Memory::PatchBytes(HostExistForPauseAddr, jmp, sizeof(jmp));
    }
}

static void PatchPauseH3ODST(HMODULE Module)
{
    const uintptr_t HostExistForPauseAddr = FindAndPrintPatternW(L"4d 85 c9 0f 84 ? ? ? ? 38 1d ? ? ? ? 0f 85 ? ? ? ?", __FUNCTIONW__ "_HostExistForPause", 3);
    if (HostExistForPauseAddr)
    {
        Memory::PatchBytes(HostExistForPauseAddr, jmp, sizeof(jmp));
    }
}

void PatchHalo3(HMODULE Module, bool isOdst)
{
    if (Module)
    {
        if (ConfigSettings.IsHalo3MainMenuLevelExist(isOdst))
        {
            PatchMainMenu(Module, isOdst);
        }
        if (ConfigSettings.bEnablePauseMenuHalo3)
        {
            PatchPauseH3(Module);
        }
        if (ConfigSettings.bEnablePauseMenuHalo3ODST && isOdst)
        {
            PatchPauseH3ODST(Module);
        }
    }
}
