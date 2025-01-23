#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

const wchar_t* g_ProgramName = L"" _PROJECT_NAME;

static void DisableSharpness()
{
    const unsigned char patch[] = { 0x0F, 0x57, 0xC0, 0x90, 0x90 };
    WritePatchPattern(L"E8 ? ? ? ? 43 8D 04 12 0F 28 D0", patch, sizeof(patch), L"Disable Sharpness", 0);
}

static DWORD __stdcall Main(void*)
{
    baseModule = GetModuleHandle(L"DD2.exe");
    if (baseModule)
    {
        DisableSharpness();
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
            return Main(0);
        }
        default:
        {
            break;
        }
    }
    return TRUE;
}
