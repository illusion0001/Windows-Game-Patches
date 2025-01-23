#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule {};

static void Main2()
{
    uintptr_t LogoVideosByte = FindAndPrintPatternW(L"80 3d ba ? ? ? ? 48 89 b4 24 c0 00 00 00 48 89 bc 24 c8 00 00 00 0f 85 ? ? ? ? 80 3d a1 ? ? ? ? 0f 85 ? ? ? ? 80 3d 98 ? ? ? ? 74 ?", L"LogoVideosByte");
    if (LogoVideosByte)
    {
        uint8_t byte1[] = { 0x1 };
        Memory::PatchBytes(ReadLEA32(LogoVideosByte+42, L"LogoVideosByte", 0, 2, 7), byte1, sizeof(byte1));
    }
}

static void* CheckThread(void*)
{
    return NULL;
}

static HANDLE CreateThread_1(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags,
    LPDWORD lpThreadId)
{
    // 0x1b8e0  246  CreateThread
    static int CreateThreadCount {};
#ifdef _DEBUG
    LOG(L"CreateThreadCount: %d\n", CreateThreadCount);
    LOG(L"lpStartAddress: 0x%p\n", lpStartAddress);
#endif
    if (CreateThreadCount == 1)
    {
        Main2();
    }
    else if (CreateThreadCount == 154)
    {
        // need a valid handle so return a fake one
        lpStartAddress = (LPTHREAD_START_ROUTINE)&CheckThread;
    }
    CreateThreadCount++;
    return CreateRemoteThreadEx
    ((HANDLE)0xffffffffffffffff, lpThreadAttributes, dwStackSize, lpStartAddress,
        lpParameter, dwCreationFlags & 0x10004, (LPPROC_THREAD_ATTRIBUTE_LIST)0x0,
        lpThreadId);
}

static void ShowConsole()
{
#ifdef _DEBUG
    AllocConsole();
    SetConsoleTitle(L"" PROJECT_NAME " - Debug Console");
    freopen_s(reinterpret_cast<FILE**>(stdin), "conin$", "r", stdin);
    freopen_s(reinterpret_cast<FILE**>(stdout), "conout$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stderr), "conout$", "w", stderr);
    ShowWindow(GetConsoleWindow(), SW_SHOW);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hConsole, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, dwMode);
#endif
}

static void Main()
{
    baseModule = GetModuleHandle(L"ACU.exe");
    if (!baseModule)
    {
        return;
    }
    ShowConsole();
    uintptr_t CreateThread_Addr = (uintptr_t)&CreateThread;
    Memory::DetourFunction64((void*)CreateThread_Addr, (void*)&CreateThread_1, 14);
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
        Main();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
