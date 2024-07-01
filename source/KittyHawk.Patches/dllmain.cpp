#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "git_ver.h"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "KittyHawk.Patches"

// INI Variables
uint32_t injection_delay{};

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Initialize config
    std::wstring config_path = L"" PROJECT_NAME ".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        std::wstring ini_defaults = L"[Settings]\n"
            wstr(injection_delay)" = 2\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        injection_delay = 2;
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(injection_delay), injection_delay);
    }
}

bool CreateConsoleHook2(HWND ConsoleWindow)
{
    ShowWindow(ConsoleWindow, 1);
    return 1;
}

uintptr_t g_GlobalHandle{};

void __attribute__((naked)) CreateConsoleHookAsm()
{
    __asm
    {
        MOV RAX, qword ptr[g_GlobalHandle];
        MOV RAX, qword ptr[RAX];
        MOVZX EDX, CL;
        ADD EDX, EDX;
        MOV dword ptr[RAX + 0xa504], EDX;
        MOV byte ptr[RAX + 0xa4e4], 0x4;
        MOV RCX, qword ptr[RAX + 0xa5d0];
        CALL CreateConsoleHook2;
        RET;
    }
}

#define TYPEDEF_FUNCTION_PTR(ret_type, func_name, ...) \
    typedef ret_type (*func_name##_ptr)(__VA_ARGS__); \
    extern func_name##_ptr func_name;

#define INIT_FUNCTION_PTR(func_name) \
    func_name##_ptr func_name = nullptr

void RegisterCustomCmd()
{
}

void DoPatches()
{
    uintptr_t DevMode_EnableConsoleAddr = FindAndPrintPatternW(L"48 8b 05 ? ? ? ? 0f b6 d1 03 d2 89 90 04 a5 00 00 c3", L"Enable Console Hook", 0);
    g_GlobalHandle = ReadLEA32(DevMode_EnableConsoleAddr, L"Enable Console Hook", 0, 3, 7);
    if (g_GlobalHandle && DevMode_EnableConsoleAddr)
    {
        Memory::DetourFunction64((void*)DevMode_EnableConsoleAddr, (void*)&CreateConsoleHookAsm, 14);
    }
}

DWORD __stdcall Main(void*)
{
    baseModule = GetModuleHandle(NULL);
    ReadConfig();
    // this isn't particularly reliable
    // due to arxan unpacking
    // only matters for patch that does its thing during startup
    if (injection_delay)
    {
        Sleep(injection_delay * 1000);
    }
    DoPatches();
    return true;
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
        CreateThread(NULL, 0, Main, 0, NULL, 0);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
