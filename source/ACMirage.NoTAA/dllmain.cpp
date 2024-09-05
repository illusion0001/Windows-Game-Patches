#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"ACMirage.NoTAA"

// INI Variables
bool bDisableTAA {};
bool bDisableCA {};

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;

    // Initialize config
    // UE4 games use launchers so config path is relative to launcher
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        std::wstring ini_defaults = L"[Settings]\n"
                                    wstr(bDisableTAA)" = true\n"
                                    wstr(bDisableCA)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableTAA = true;
        bDisableCA = true;
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableCA),  bDisableCA);
    }
}

void DisableTAA(void)
{
    const unsigned char patch[] = { 0x41, 0xC6, 0x80, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x87, 0xD0, 0x00, 0x00, 0x00 };
    //  00007FF635E8D3F0 | 41:C680 A0000000 01  | mov byte ptr ds : [r8 + A0] , 1       --> 41:C680 A0000000 01  | mov byte ptr ds : [r8 + A0] , 0
    //  00007FF635E8D3F8 | 8B87 D0000000        | mov eax, dword ptr ds : [rdi + D0]
    WritePatchPattern(L"41 C6 80 A0 00 00 00 01 8B 87 D0 00 00 00", patch, sizeof(patch), L"Disable TAA", 0);
}

void DisableCA(void)
{
    const unsigned char patch[] = { 0x80, 0x79, 0x32, 0x01, 0x75, 0x4E, 0x49, 0x8B, 0xCE };
    //  00007FF634C07795 | 8079 32 00 | cmp byte ptr ds : [rcx + 32] , 0                --> 8079 32 01 | cmp byte ptr ds : [rcx + 32] , 1 
    //  00007FF634C07799 | 75 4E | jne acmirage.7FF634C077E9 
    //  00007FF634C0779B | 49 : 8BCE | mov rcx, r14 
    WritePatchPattern(L"80 79 32 00 75 4E 49 8B CE", patch, sizeof(patch), L"Disable CA", 0);
}

DWORD __stdcall Main(void*)
{
    ReadConfig();

    if (bDisableTAA)
        DisableTAA();
    if (bDisableCA)
        DisableCA();

    return true;
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
        CreateThread(NULL, 0, Main, 0, NULL, 0);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
