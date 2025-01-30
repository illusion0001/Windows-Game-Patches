#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "SDKEnums.hpp"

#include "function_ptr.h"

#include <d3d11.h>
#include "d3d11_patch.hpp"

HMODULE baseModule{};

static int my_printf(const char* fmt, ...)
{
    char temp_buffer[32 * 1024]{};
    va_list args{};
    va_start(args, fmt);
    int len = vsnprintf_s(temp_buffer, _countof(temp_buffer), _TRUNCATE, fmt, args);
    va_end(args);
    printf_s("%s\n", temp_buffer);
    return len;
}

INIT_FUNCTION_PTR(Present_Original);
INIT_FUNCTION_PTR(ResizeBuffers_Original);
INIT_FUNCTION_PTR(CreateD3DDevice_Original);

static void DoPatchRenderer()
{
    const uintptr_t CreateD3DDevice = FindAndPrintPatternW(L"4c 8d 4c 24 ? 48 8b 54 24 ? 4d 8b c7 48 89 44 24 ? 48 8b ce 48 8d 44 24 ? 48 89 44 24 ? e8 ? ? ? ?", L"CreateD3DDevice", 31);
    if (CreateD3DDevice)
    {
        const uintptr_t int3jmp = FindInt3Jmp();
        if (int3jmp)
        {
            CreateD3DDevice_Original = (CreateD3DDevice_Original_ptr)ReadLEA32(CreateD3DDevice, L"CreateD3DDevice_Original", 0, 1, 5);
            MAKE32CALL(CreateD3DDevice, int3jmp, CreateD3DDevice_Hook, 5);
        }
    }
}

FLAG_CAT* g_Cat;

static void DoPatches()
{
    const uintptr_t setDebugSaveMaskAddr = FindAndPrintPatternW(L"81 0d ? ? ? ? 00 00 00 08 4c 89 64 ? ?", L"setDebugSaveMask");
    if (setDebugSaveMaskAddr)
    {
        static const uint32_t newFlag = 0;
        Memory::PatchBytes(setDebugSaveMaskAddr + 6, &newFlag, sizeof(newFlag));
    }
    const uintptr_t nullputs = FindAndPrintPatternW(L"48 89 54 24 10 4c 89 44 24 18 4c 89 4c 24 20 c3", L"nullputs");
    if (nullputs)
    {
        Memory::DetourFunction64(nullputs, (uintptr_t)my_printf, 14);
    }
    const uintptr_t flag_cat_ptr = FindAndPrintPatternW(L"48 89 4b 18 48 8b 01 ff 50 08 48 8d 05 ? ? ? ? 4c 8d 1d ? ? ? ?", L"flag_cat_ptr", 17);
    if (flag_cat_ptr)
    {
        g_Cat = (FLAG_CAT*)ReadLEA32(flag_cat_ptr, L"g_flag_cat_ptr", 0, 3, 7);
        for (size_t i = 0; i < FLAG_CATEGORY::MAX; i++)
        {
            LOG("g_Cat[%lld].cat_list: 0x%p\n", i, g_Cat[i].cat_list);
            LOG("g_Cat[%lld].cat_name: %hs\n", i, g_Cat[i].cat_name);
        }
    }
    const uintptr_t damageMulAddr = FindAndPrintPatternW(L"0f ba e0 1a 73 ? 8d 34 b6 44 8b f6", L"damageMulAddr");
    if (damageMulAddr)
    {
        const uintptr_t int3jmp = FindInt3Jmp();
        if (int3jmp)
        {
            static uint8_t cave[] =
            {
                0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, // movabs rdx, 0x1000000000000000
                0x49, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, // movabs r14, 0x2000000000000000
                0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, // movabs rcx, 0x3000000000000000
                0x0F, 0xBA, 0xE0, 0x1A, // bt eax, 0x1a
                0x73, 0x27, // jae 0x4b
                0x8B, 0x12, // mov edx, dword ptr [rdx]
                0x85, 0xD2, // test edx, edx
                0x74, 0x1B, // je 0x45
                0x83, 0xFA, 0x01, // cmp edx, 1
                0x74, 0x07, // je 0x36
                0x83, 0xFA, 0x02, // cmp edx, 2
                0x74, 0x0B, // je 0x3f
                0xEB, 0x0F, // jmp 0x45
                0x45, 0x8B, 0x36, // mov r14d, dword ptr [r14]
                0x41, 0x0F, 0xAF, 0xF6, // imul esi, r14d
                0xEB, 0x09, // jmp 0x48
                0x8B, 0x09, // mov ecx, dword ptr [rcx]
                0x89, 0xCE, // mov esi, ecx
                0xEB, 0x03, // jmp 0x48
                0x8D, 0x34, 0xB6, // lea esi, [rsi + rsi*4]
                0x41, 0x89, 0xF6, // mov r14d, esi
            };
            extern int g_DamageMode;
            extern int g_DamageMul;
            extern int g_DamageConst;
            Memory::nPatchBytes(cave + 0 + 2, &g_DamageMode, sizeof(&g_DamageConst), true);
            Memory::nPatchBytes(cave + 10 + 2, &g_DamageMul, sizeof(&g_DamageMul), true);
            Memory::nPatchBytes(cave + 10 + 10 + 2, &g_DamageConst, sizeof(&g_DamageConst), true);
            const size_t retaddr = 12;
            const uintptr_t caveDst = Memory::CreateCodeCaveBlockX64(cave, damageMulAddr + retaddr, sizeof(cave));
            MAKE32HOOK(damageMulAddr, int3jmp, caveDst, retaddr);
        }
    }
}

static BOOL Main()
{
    baseModule = GetModuleHandle(L"NieRAutomata.exe") ? : GetModuleHandle(L"NieRAutomataCompat.exe");
    if (baseModule)
    {
        ShowConsole1();
        DoPatchRenderer();
        DoPatches();
        return true;
    }
    return false;
}

BOOL APIENTRY DllMain(HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            return Main();
        }
        default:
        {
            break;
        }
    }
    return TRUE;
}
