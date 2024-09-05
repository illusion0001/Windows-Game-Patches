#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule {};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define EXE_NAME "Dead Space.exe"
#define PROJECT_NAME "DeadSpace.NoTAA"
#define PROJECT_LOG_PATH PROJECT_NAME ".log"

static void DisableTAA()
{
    // This will stop working when game is updated and RIP is shifted
    const uint8_t PostAA_Mode0[] = { 0x83, 0xC1, 0x07, 0x90, 0x90, 0x48, 0x63, 0xC1 };
    WritePatchPattern(L"83 F9 07 77 56 48 63 C1 48 8D 15 C1 31 4F FC", PostAA_Mode0, sizeof(PostAA_Mode0), L"Disable TAA", 0);
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
