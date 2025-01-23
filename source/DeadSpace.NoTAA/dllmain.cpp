#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule {};

#define EXE_NAME "Dead Space.exe"
#define PROJECT_LOG_PATH PROJECT_NAME ".log"

static uintptr_t g_PostProcessPtr{};
static int g_AntiAliasOffset{};

static void __attribute__((naked)) WritePostProcessPtrAsm()
{
    __asm
    {
        // rax will be overwriten so lets use it
        // read original ptr
        mov rax, qword ptr[g_PostProcessPtr];
        // write it back
        mov qword ptr[rax], rcx;
        // read PostProcess AntiAliasing offset
        movsxd rax, dword ptr[g_AntiAliasOffset];
        // write it with None
        mov dword ptr[rcx + rax], 0;
        ret;
    }
}

static void DisableTAA()
{
    const uintptr_t WritePostProcessPtr = FindAndPrintPatternW(L"48 89 0D ? ? ? ? 48 8B 83 ? ? ? ? 48 85 C0", L"WritePostProcessPtr");
    const uintptr_t ReadAntiAliasOffset = FindAndPrintPatternW(L"8B 8B ? ? ? ? 8D 41 FF 83 F8 02", L"ReadAntiAliasOffset");
    if (WritePostProcessPtr && ReadAntiAliasOffset)
    {
        g_PostProcessPtr = ReadLEA32(WritePostProcessPtr, L"g_PostProcessPtr", 0, 3, 7);
        Memory::ReadBytes(ReadAntiAliasOffset + 2, &g_AntiAliasOffset, sizeof(g_AntiAliasOffset));
        const uintptr_t int3 = FindAndPrintPatternW(L"C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC", L"ret;int3", 1);
        if (g_PostProcessPtr && g_AntiAliasOffset && int3)
        {
            MAKE32CALL(WritePostProcessPtr, int3, &WritePostProcessPtrAsm, 7);
        }
    }
}

static void* Main(void* ptr)
{
    static bool ranOnce {};
    if (!ranOnce)
    {
        baseModule = GetModuleHandle(L"" EXE_NAME);
        if (!baseModule)
        {
            MessageBox(0, L"Unable to find `" EXE_NAME "` module", L"" PROJECT_NAME " Startup Error", MB_ICONERROR | MB_OK);
            ExitProcess(0);
            return ptr;
        }
        DisableTAA();
        ranOnce = true;
    }
    return ptr;
}

static void Hook_SetUnhandledExceptionFilter()
{
    void* SetUnhandledExceptionFilterAddr = (void*)&SetUnhandledExceptionFilter;
    Memory::DetourFunction64(SetUnhandledExceptionFilterAddr, (void*)&Main, 14);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        Hook_SetUnhandledExceptionFilter();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
