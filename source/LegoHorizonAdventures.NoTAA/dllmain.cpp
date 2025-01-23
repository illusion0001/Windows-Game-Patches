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
                                    wstr(bDisableTAA)" = true\n"
                                    wstr(bDisableDoF)" = true\n";
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
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableDoF), bDisableDoF);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDisableTAA), GetBoolStr(bDisableTAA) , bDisableTAA);
    LOG(L"%s: %s (%i)\n", wstr(bDisableDoF), GetBoolStr(bDisableDoF), bDisableDoF);
}

void DisableTAA(void)
{
    const unsigned char patch[] = { 0x31, 0xC9, 0x90, 0x90 }; 
    // EB02       | jmp legohorizonadventures-win64-shipping. 
    // 33C0       | xor eax, eax 
    // B905000000 | mov ecx, 5 
    // 390C38     | cmp dword ptr ds : [rax + rdi] , ecx
    // 0F4C0C38   | cmovl ecx, dword ptr ds : [rax + rdi]             --> 31C9 | xor ecx,ecx
    //                                                                      90 | nop 
    //                                                                      90 | nop
    // 33FF       | xor edi, edi 
    WritePatchPattern(L"B9 05 00 00 00 39 0C 38 0F 4C 0C 38", patch, sizeof(patch), L"Disable TAA", 8);    //sets r.AntiAliasingMethod=0
    WritePatchPattern(L"B9 05 00 00 00 39 0C 38 0F 4C 0C 38", patch, sizeof(patch), L"Disable TAA", 8);    //sets r.Mobile.AntiAliasing=0 (not required, but the sub body is the same)
}

void DisableDoF(void)
{
    const unsigned char patch[] = { 0x31, 0xFF, 0x90 };
    // 7505         | jne legohorizonadventures-win64-shipping. 
    // BF04000000   | mov edi, 4
    // 8B3C37       | mov edi, dword ptr ds : [rdi + rsi]           --> 31FF | xor edi,edi
    //                                                                    90 | nop
    // 488BCB       | mov rcx, rbx 
    WritePatchPattern(L"BF 04 00 00 00 8B 3C 37 48 8B CB", patch, sizeof(patch), L"Disable DoF", 5);    //sets r.DepthOfFieldQuality=0
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
    if (bDisableDoF)
        DisableDoF();

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
