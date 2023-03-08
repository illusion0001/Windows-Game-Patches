#include "stdafx.h"
#include "helper.hpp"

using namespace std;

HMODULE baseModule = GetModuleHandle(NULL);

#define GAME_NAME "BrightMemoryInfinite"
#define xstr(s) str(s)
#define str(s) #s

inipp::Ini<char> ini;
FILE* fp_log;
time_t sec;

#define LOG(fmt,...)                                                                \
{                                                                                   \
    time(&sec);                                                                     \
    fprintf(fp_log, "%s: %.24s " fmt "\n", __FUNCTION__, ctime(&sec), __VA_ARGS__); \
}

// INI Variables
bool bDisableTAA;
bool bDisableSharpness;

void LoggingInit(void)
{
    const char* log_path = GAME_NAME ".log";
    time(&sec);
    fp_log = fopen(log_path, "w");
    if (fp_log != NULL)
    {
        LOG("Log file opened at %s", log_path);
    }
    else
    {
        MessageBox(0, L"Failed to open log file.", (wchar_t*)GAME_NAME, MB_ICONWARNING);
    }
}

void ReadConfig(void)
{
    string sExePath;
    string sExeName;
    string sGameName;
    // Get game name and exe path
    LPWSTR exePath = new WCHAR[_MAX_PATH];
    GetModuleFileNameW(baseModule, exePath, _MAX_PATH);
    wstring exePathWString(exePath);
    sExePath = string(exePathWString.begin(), exePathWString.end());
    wstring wsGameName = Memory::GetVersionProductName();
    sExeName = sExePath.substr(sExePath.find_last_of("/\\") + 1);
    sGameName = string(wsGameName.begin(), wsGameName.end());

    LOG("Game Name: %s", sGameName.c_str());
    LOG("Game Path: %s", sExePath.c_str());

    // Initialize config
    // UE4 games use launchers so config path is relative to launcher
    std::string config_path = GAME_NAME ".ini";
    std::ifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG("Failed to load config file.");
        string ini_defaults = "[Settings]\n"
                              str(bDisableTAA)" = true\n"
                              str(bDisableSharpness)" = true\n";
        std::ofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableTAA = true;
        bDisableSharpness = true;
        LOG("Created default config file.");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections["Settings"], str(bDisableTAA), bDisableTAA);
        inipp::get_value(ini.sections["Settings"], str(bDisableSharpness), bDisableSharpness);
    }

    // Log config parse
    LOG("%s: %s (%i)", str(bDisableTAA), bDisableTAA ? "true" : "false", bDisableTAA);
    LOG("%s: %s (%i)", str(bDisableSharpness), bDisableSharpness ? "true" : "false", bDisableSharpness);
}

void ShowPatchInfo(uint32_t Patch_Length, size_t Patch_Size, uint64_t Patch_Addr, const char* Patch_Name)
{
    LOG("Patch Name: %s", Patch_Name);
    LOG("Hook length: %u bytes", Patch_Length);
    LOG("Patch length: %llu bytes", Patch_Size);
    LOG("Hook address: 0x%016llx", Patch_Addr);
}

void DisableTAA(void)
{
    uint8_t* DisableTAAResult = Memory::PatternScan(baseModule, "8B 04 8E 83 F8 ?? 77 ?? 89 83 ?? ?? ?? ??");
    if (DisableTAAResult)
    {
        const char xor_eax_eax[] = { '\x31', '\xc0', '\x90' };
        uint64_t DisableTAAAddress = ((uintptr_t)DisableTAAResult);
        uint32_t DisableTAALength = Memory::GetHookLength((char*)DisableTAAAddress, 14);
        Memory::PatchBytes((uintptr_t)DisableTAAAddress, xor_eax_eax, sizeof(xor_eax_eax));
        ShowPatchInfo(DisableTAALength, sizeof(xor_eax_eax), (uintptr_t)DisableTAAAddress, str(bDisableTAA));
    }
    else
    {
        LOG("Pattern scan failed.");
    }
}

void DisableSharpness(void)
{
    uint8_t* DisableSharpnessResult = Memory::PatternScan(baseModule, "F3 44 0F 10 40 04 44 0F 2F C6 73 ?? 45 0F 57 C0 EB ??");
    if (DisableSharpnessResult)
    {
        const char xor_xmm8[] = { '\x45', '\x0F', '\x57', '\xC0', '\x90', '\x90' };
        uint64_t DisableSharpnessAddress = ((uintptr_t)DisableSharpnessResult);
        uint32_t DisableSharpnessLength = Memory::GetHookLength((char*)DisableSharpnessAddress, 18);
        Memory::PatchBytes((uintptr_t)DisableSharpnessAddress, xor_xmm8, sizeof(xor_xmm8));
        ShowPatchInfo(DisableSharpnessLength, sizeof(xor_xmm8), (uintptr_t)DisableSharpnessAddress, str(bDisableSharpness));
    }
    else
    {
        LOG("Pattern scan failed.");
    }
}

DWORD __stdcall Main(void*)
{
    bDisableTAA = false;
    bDisableSharpness = false;
    LoggingInit();
    ReadConfig();
    if (bDisableTAA)
        DisableTAA();
    if (bDisableSharpness)
        DisableSharpness();
    LOG("Shutting down " str(fp_log) " file handle.");
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
