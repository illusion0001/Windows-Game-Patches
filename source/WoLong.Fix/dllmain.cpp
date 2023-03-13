#include "stdafx.h"
#include "helper.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"WoLong.Fix"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

FILE* fp_log;
std::time_t current_time;
struct tm timeinfo;

// INI Variables
bool bLoggingEnabled;
bool bRemove30FPSCutscenes;
bool bDisableTAA;

void log_time(void)
{
    wchar_t wtime[256];
    if (localtime_s(&timeinfo, &current_time) == 0 && std::wcsftime(wtime, _countof(wtime), L"%A %c", &timeinfo))
        fwprintf_s(fp_log, L"%-32s ", wtime);
}

void file_log(const wchar_t* fmt, ...)
{
    if (!bLoggingEnabled)
        return;

    log_time();
    va_list args;
    va_start(args, fmt);
    vfwprintf_s(fp_log, fmt, args);
    va_end(args);
}

const wchar_t* GetBoolStr(bool input_bool)
{
    return input_bool ? L"true" : L"false";
}

#define LOG(fmt, ...) file_log(L"%-24s:%u " fmt "\n", __FUNCTIONW__, __LINE__, __VA_ARGS__);

void LoggingInit(void)
{
    errno_t file_stat = _wfopen_s(&fp_log, _PROJECT_LOG_PATH, L"w+, ccs=UTF-8");
    if (file_stat == 0)
    {
        bLoggingEnabled = true;
        std::locale::global(std::locale("en_US.utf8"));
        current_time = std::time(nullptr);
        LOG(L"Log file opened at " _PROJECT_LOG_PATH);
    }
    else
    {
        bLoggingEnabled = false;
        wchar_t errorText[256] = { 0 };
        wchar_t errorMsg[512] = { 0 };
        _wcserror_s(errorText, _countof(errorText), file_stat);
        _snwprintf_s(errorMsg, _countof(errorMsg), _TRUNCATE, L"Failed to open log file. (%s)\nError code: %i (0x%x) %s", _PROJECT_LOG_PATH, file_stat, file_stat, errorText);
        MessageBox(0, errorMsg, _PROJECT_NAME, MB_ICONWARNING);
    }
}

void ReadConfig(void)
{
    inipp::Ini<wchar_t> ini;
    // Get game name and exe path
    LPWSTR exePath = new WCHAR[_MAX_PATH];
    GetModuleFileNameW(baseModule, exePath, _MAX_PATH);
    std::wstring exePathWString(exePath);
    std::wstring wsGameName = Memory::GetVersionProductName();

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
        std::wstring ini_defaults = L"[Settings]\n"
                                    "; " wstr(bRemove30FPSCutscenes) " causes double speed, needs more research.\n"
                                    wstr(bRemove30FPSCutscenes)" = false\n"
                                    wstr(bDisableTAA)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bRemove30FPSCutscenes = false;
        bDisableTAA = true;
        LOG(L"Created default config file.");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bRemove30FPSCutscenes), bRemove30FPSCutscenes);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableTAA), bDisableTAA);
    }

    // Log config parse
    LOG(L"%s: %s (%i)", wstr(bRemove30FPSCutscenes), GetBoolStr(bRemove30FPSCutscenes) , bRemove30FPSCutscenes);
    LOG(L"%s: %s (%i)", wstr(bDisableTAA), GetBoolStr(bDisableTAA), bDisableTAA);
}

void ShowPatchInfo(size_t Hook_Length, size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name)
{
    LOG(L"Patch Name: %s", Patch_Name);
    if (Hook_Length)
        LOG(L"Hook length: %u bytes", Hook_Length);
    LOG(L"Patch length: %llu bytes", Patch_Size);
    LOG(L"Patch address: 0x%016llx", Patch_Addr);
}

void Remove30FPSCutscenes(void)
{
    uint8_t* Write30FPSCutscenesResult = Memory::PatternScan(baseModule, "C7 80 ?? ?? ?? ?? 1E 00 00 00 48 8B ?? ?? ?? ?? ?? C7 80 ?? ?? ?? ?? 1E 00 00 00 48 8B 5D 00 48 85 DB");
    if (Write30FPSCutscenesResult)
    {
        const unsigned char mov_ecx[] = { 0x89, 0x88 };
        const unsigned char nop4x[] = { 0x90, 0x90, 0x90, 0x90 };
        uint64_t Write30FPSAddress = ((uintptr_t)Write30FPSCutscenesResult);
        Memory::PatchBytes((uintptr_t)Write30FPSAddress, mov_ecx, sizeof(mov_ecx));
        Memory::PatchBytes((uintptr_t)Write30FPSAddress + 17, mov_ecx, sizeof(mov_ecx));
        Memory::PatchBytes((uintptr_t)Write30FPSAddress + 6, nop4x, sizeof(nop4x));
        Memory::PatchBytes((uintptr_t)Write30FPSAddress + 23, nop4x, sizeof(nop4x));
        ShowPatchInfo(0, sizeof(mov_ecx) + sizeof(nop4x), (uintptr_t)Write30FPSAddress, wstr(bRemove30FPSCutscenes));
    }
    else
    {
        LOG(L"Pattern scan failed.");
    }
}

void DisableTAA(void)
{
    // Still has sharpening filter
    uint8_t* TAABoolResult = Memory::PatternScan(baseModule, "44 8B 00 45 85 C0 0F 9F C0 88 41 10 45 85 C0 0F 8E ?? ?? ?? ?? 4C 8B 81 00 01 00 00");
    if (TAABoolResult)
    {
        const unsigned char xor_r8d[] = { 0x45, 0x31, 0xc0 };
        uint64_t TAABoolAddress = ((uintptr_t)TAABoolResult);
        Memory::PatchBytes((uintptr_t)TAABoolAddress, xor_r8d, sizeof(xor_r8d));
        ShowPatchInfo(0, sizeof(xor_r8d), (uintptr_t)TAABoolAddress, wstr(bDisableTAA));
    }
    else
    {
        LOG(L"Pattern scan failed.");
    }
}

DWORD __stdcall Main(void*)
{
    bLoggingEnabled = false;
    bRemove30FPSCutscenes = false;
    bDisableTAA = false;
    LoggingInit();
    ReadConfig();
    if (bRemove30FPSCutscenes)
        Remove30FPSCutscenes();
    if (bDisableTAA)
        DisableTAA();
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
