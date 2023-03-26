#include "stdafx.h"
#include "helper.hpp"

HMODULE baseModule = GetModuleHandle(NULL);

#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"RE4.Sharpness"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

FILE* fp_log;
std::time_t current_time;
struct tm timeinfo;

// INI Variables
bool bLoggingEnabled;
bool bRemoveSharpness;

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
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.");
        std::wstring ini_defaults = L"[Settings]\n"
                                    wstr(bRemoveSharpness)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bRemoveSharpness = true;
        LOG(L"Created default config file.");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bRemoveSharpness), bRemoveSharpness);
    }

    // Log config parse
    LOG(L"%s: %s (%i)", wstr(bRemoveSharpness), GetBoolStr(bRemoveSharpness) , bRemoveSharpness);
}

void ShowPatchInfo(size_t Hook_Length, size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name)
{
    LOG(L"Patch Name: %s", Patch_Name);
    if (Hook_Length)
        LOG(L"Hook length: %u bytes", Hook_Length);
    LOG(L"Patch length: %llu bytes", Patch_Size);
    LOG(L"Patch address: 0x%016llx", Patch_Addr);
}

void RemoveSharpness(void)
{
    // Thanks to TL431
    uint8_t* TAAFlagResult = Memory::PatternScan(baseModule, "8B 56 28 48 8B CB E8 ?? ?? ?? ??");
    uint8_t* SharpnessFloatResult = Memory::PatternScan(baseModule, "F3 0F 10 46 3C 48 8B CB 0F 5A C0 66 0F 5A C8 E8 ?? ?? ?? ??");
    if (TAAFlagResult && SharpnessFloatResult)
    {
        const unsigned char patch1[] = { 0x31, 0xd2, 0x90 };
        const unsigned char patch2[] = { 0x0F, 0x57, 0xC0, 0x90, 0x90 };
        uint64_t TAAFlagResultAddress = ((uintptr_t)TAAFlagResult);
        Memory::PatchBytes((uintptr_t)TAAFlagResultAddress, patch1, sizeof(patch1));
        ShowPatchInfo(0, sizeof(patch1), (uintptr_t)TAAFlagResultAddress, wstr(bRemoveSharpness));
        uint64_t SharpnessFloatResultAddress = ((uintptr_t)SharpnessFloatResult);
        Memory::PatchBytes((uintptr_t)SharpnessFloatResultAddress, patch2, sizeof(patch2));
        ShowPatchInfo(0, sizeof(patch2), (uintptr_t)SharpnessFloatResultAddress, wstr(bRemoveSharpness));
    }
    else
    {
        LOG(L"Pattern scan failed.");
    }
}

DWORD __stdcall Main(void*)
{
    Sleep(5 * 1000); // Bypass the checksum protection
    bLoggingEnabled = false;
    bRemoveSharpness = false;
    LoggingInit();
    ReadConfig();
    if (bRemoveSharpness)
        RemoveSharpness();
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
