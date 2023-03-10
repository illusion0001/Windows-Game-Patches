#include "stdafx.h"
#include "helper.hpp"

using namespace std;

HMODULE baseModule = GetModuleHandle(NULL);

#define str(s) #s
#define xstr(s) str(s)
#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define PROJECT_NAME "BrightMemoryInfinite.NoTAA"
#define _PROJECT_NAME L"BrightMemoryInfinite.NoTAA"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

FILE* fp_log;

// INI Variables
bool bLoggingEnabled;
bool bDisableTAA;
bool bDisableSharpness;

void log_time(void)
{
    std::locale::global(std::locale("en_US.utf8"));
    std::time_t t = std::time(nullptr);
    wchar_t wtime[256];
    if (std::wcsftime(wtime, sizeof(wtime), L"%A %c", std::localtime(&t)))
        fwprintf(fp_log, L"%s ", wtime);
}

void file_log(const wchar_t* fmt, ...)
{
    if (bLoggingEnabled)
    {
        log_time();
        va_list args;
        va_start(args, fmt);
        vfwprintf(fp_log, fmt, args);
        va_end(args);
    }
}

#define LOG(fmt, ...) file_log(L"%s:%u " fmt "\n", __FUNCTIONW__, __LINE__, __VA_ARGS__);

void LoggingInit(void)
{
    fp_log = _wfopen(_PROJECT_LOG_PATH, L"w");
    if (fp_log != NULL)
    {
        LOG(L"Log file opened at " _PROJECT_LOG_PATH);
        bLoggingEnabled = true;
    }
    else
    {
        MessageBox(0, L"Failed to open log ( " _PROJECT_LOG_PATH " ) file.", _PROJECT_NAME, MB_ICONWARNING);
        bLoggingEnabled = false;
    }
}

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LPWSTR exePath = new WCHAR[_MAX_PATH];
    GetModuleFileNameW(baseModule, exePath, _MAX_PATH);
    wstring exePathWString(exePath);
    wstring wsGameName = Memory::GetVersionProductName();

    LOG(_PROJECT_NAME " Built: " __TIME__ " @ " __DATE__);
    LOG(L"Game Name: %s", wsGameName.c_str());
    LOG(L"Game Path: %s", exePathWString.c_str());

    // Initialize config
    // UE4 games use launchers so config path is relative to launcher
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.");
        wstring ini_defaults = L"[Settings]\n"
                              wstr(bDisableTAA)" = true\n"
                              wstr(bDisableSharpness)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableTAA = true;
        bDisableSharpness = true;
        LOG(L"Created default config file.");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableSharpness), bDisableSharpness);
    }

    // Log config parse
    LOG(L"%s: %s (%i)", wstr(bDisableTAA), bDisableTAA ? L"true" : L"false", bDisableTAA);
    LOG(L"%s: %s (%i)", wstr(bDisableSharpness), bDisableSharpness ? L"true" : L"false", bDisableSharpness);
}

void ShowPatchInfo(size_t Hook_Length, size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name)
{
    LOG(L"Patch Name: %s", Patch_Name);
    LOG(L"Hook length: %u bytes", Hook_Length);
    LOG(L"Patch length: %llu bytes", Patch_Size);
    LOG(L"Hook address: 0x%016llx", Patch_Addr);
}

void DisableTAA(void)
{
    uint8_t* DisableTAAResult = Memory::PatternScan(baseModule, "8B 04 8E 83 F8 ?? 77 ?? 89 83 ?? ?? ?? ??");
    if (DisableTAAResult)
    {
        const unsigned char xor_eax_eax[] = { 0x31, 0xc0, 0x90 };
        uint64_t DisableTAAAddress = ((uintptr_t)DisableTAAResult);
        Memory::PatchBytes((uintptr_t)DisableTAAAddress, xor_eax_eax, sizeof(xor_eax_eax));
        ShowPatchInfo(0, sizeof(xor_eax_eax), (uintptr_t)DisableTAAAddress, wstr(bDisableTAA));
    }
    else
    {
        LOG(L"Pattern scan failed.");
    }
}

void DisableSharpness(void)
{
    uint8_t* DisableSharpnessResult = Memory::PatternScan(baseModule, "F3 44 0F 10 40 04 44 0F 2F C6 73 ?? 45 0F 57 C0 EB ??");
    if (DisableSharpnessResult)
    {
        const unsigned char xor_xmm8[] = { 0x45, 0x0f, 0x57, 0xc0, 0x90, 0x90 };
        uint64_t DisableSharpnessAddress = ((uintptr_t)DisableSharpnessResult);
        Memory::PatchBytes((uintptr_t)DisableSharpnessAddress, xor_xmm8, sizeof(xor_xmm8));
        ShowPatchInfo(0, sizeof(xor_xmm8), (uintptr_t)DisableSharpnessAddress, wstr(bDisableSharpness));
    }
    else
    {
        LOG(L"Pattern scan failed.");
    }
}

DWORD __stdcall Main(void*)
{
    bDisableTAA = false;
    bDisableSharpness = false;
    bLoggingEnabled = false;
    LoggingInit();
    ReadConfig();
    if (bDisableTAA)
        DisableTAA();
    if (bDisableSharpness)
        DisableSharpness();
    LOG(L"Shutting down " wstr(fp_log) " file handle.");
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
