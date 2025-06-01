#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

wchar_t exePath[_MAX_PATH] = { 0 };

// INI Variables
bool bDisableTAA;
bool bDisableDoF;

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LOG(_PROJECT_NAME " Built: " __TIME__ " @ " __DATE__ "\n");
    LOG(L"Game Name: %s\n", Memory::GetVersionProductName().c_str());
    LOG(L"Game Path: %s\n", exePath);

    // Initialize config
    // UE4 games use launchers so config path is relative to launcher
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.\n");
        std::wstring ini_defaults = L"[Settings]\n"
            wstr(bDisableTAA)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableTAA = true;
        bDisableDoF = true;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDisableTAA), GetBoolStr(bDisableTAA) , bDisableTAA);
}

void DisableTAA(void)
{
    const unsigned char patch1[] = { 0xEB }; 
    // 83 78 08 00             | cmp dword ptr ds : [rax + 8] , 0 | rax + 08 
    // 74 0E                   | je thegreatcircle.7FF61AE006DF                 ->  EB 0E | jmp thegreatcircle.7FF61AE006DF
    // 48 83 3D CF 44 CF 03 00 | cmp qword ptr ds : [7FF61EAF4BA8] , 0 
    WritePatchPattern(L"83 78 08 00 74 0E 48 83 3D", patch1, sizeof(patch1), L"Disable TAA (r_TAAAntiGhosting=0)", 4);    //sets r_TAAAntiGhosting=0

    const unsigned char patch2[] = { 0xEB };
    // 83 79 08 00  | cmp dword ptr ds:[rcx+8],0
    // 7E 27        | jle thegreatcircle.7FF61AE2B404                           ->  EB 27 | jmp thegreatcircle.7FF61AE2B404
    // 4C 8B 00     | mov r8, qword ptr ds : [rax]
    WritePatchPattern(L"83 79 08 00 7E 27 4C 8B 00", patch2, sizeof(patch2), L"Disable TAA (r_antialiasing=0)", 4);    //sets r_antialiasing=0

    const unsigned char patch3[] = { 0xB8, 0x00, 0x00, 0x80, 0x3F, 0x66, 0x0F, 0x6E, 0xC8, 0x90, 0x90, 0x90 };
    // 89 74 CF 0C          | mov dword ptr ds : [rdi + rcx * 8 + C] , esi
    // 48 8B 05 1B 8B 72 03 | mov rax, qword ptr ds : [7FF61E528DE0]            -> B8 00 00 80 3F | mov eax,3F800000
    // F3 0F 10 48 0C       | movss xmm1, dword ptr ds : [rax + C]              -> 66 0F 6E C8    | movd xmm1, eax
    //                                                                          -> 90 90 90       | nop
    WritePatchPattern(L"89 74 CF 0C 48 ?? ?? ?? ?? ?? ?? F3 0F 10 48 0C", patch3, sizeof(patch3), L"Disable TAA (r_TAABlendRatio=1.0)", 4);    //sets r_TAABlendRatio=1.0
}

DWORD __stdcall Main(void*)
{
    bLoggingEnabled = false;

    wchar_t LogPath[_MAX_PATH] = { 0 };
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, L"" _PROJECT_LOG_PATH);
    LoggingInit(L"" _PROJECT_NAME, LogPath);
    ReadConfig();

    if (bDisableTAA)
        DisableTAA();

    LOG(L"Shutting down " wstr(fp_log) " file handle.\n");
    fclose(fp_log);
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
