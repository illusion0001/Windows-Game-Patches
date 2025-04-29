#include "stdafx.h"
#include <stdio.h>

#pragma once

extern HMODULE baseModule;
extern bool bLoggingEnabled;
extern FILE* fp_log;
extern FILE* fGame;
extern std::time_t current_time;
extern struct tm timeinfo;

#define str(s) #s
#define xstr(s) str(s)

#define GET_KEY(key, section, as_type, default_) key = config[section][#key].as<as_type>(default_); printf_s("" #key ": %d\n", key)
#define GET_KEY_FMT(key, section, as_type, default_, fmt) key = config[section][#key].as<as_type>(default_); printf_s("" #key ": " fmt "\n", key)
#define GET_KEY_BOOL(key, section ,as_type ,default_) key = config[section][#key].as<as_type>(default_); printf_s("" #key ": %s\n", key ? "true" : "false")
#define ADD_YAML_KEY(emit, key, val) (void)key; emit << YAML::Key << #key << YAML::Value << val; printf_s("Adding `%s` with value `%s`\n", #key, #val)

void log_time(void);
void file_log(const wchar_t* fmt, ...);
void file_log2(const wchar_t* fmt, ...);
const wchar_t* GetBoolStr(bool input_bool);
void LoggingInit(const wchar_t* Project_Name, const wchar_t* Project_Log_Name);
void ShowPatchInfo(size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name, uint64_t Patch_Function_Target);
void LogPatchFailed(const wchar_t* Patch_Name, const wchar_t* Patch_Pattern, const bool FailedQuiet);
uintptr_t WritePatchPattern(const wchar_t* Patch_Pattern, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, const bool FailedQuiet = false);
void WritePatchPattern_Hook(const wchar_t* Patch_Pattern, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, void* Function_Target, uint64_t* Return_Address, const bool FailedQuiet = false);
void WritePatchPattern_Int(uint32_t mode, const wchar_t* Patch_Pattern, void* Patch_Value, const wchar_t* Patch_Name, uint64_t Patch_Offset);
void WritePatchAddress(uint64_t Patch_Address, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset);
void WritePatchAddressFloat32(uint64_t Patch_Address, const float* Patch_Bytes, const wchar_t* Patch_Name, uint64_t Patch_Offset);
void WritePatchAddressFloat64(uint64_t Patch_Address, const double* Patch_Bytes, const wchar_t* Patch_Name, uint64_t Patch_Offset);
wchar_t* GetRunningPath(wchar_t* output);
wchar_t* GetModuleName(wchar_t* output);
uintptr_t FindAndPrintPatternW(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset = 0, const bool FailedQuiet = false);
uintptr_t FindAndPrintPatternW(const HMODULE Module, const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset = 0, const bool FailedQuiet = false);
uintptr_t FindInt3Jmp(const HMODULE Module);
uintptr_t FindInt3Jmp();
void Make32to64Call(void* source_target, void* second_jmp, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* second_jmp_name, const wchar_t* target_jmp_name);
void Make32to64Hook(void* source_target, void* second_jmp, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* second_jmp_name, const wchar_t* target_jmp_name);
void Make32Hook(void* source_target, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* target_jmp_name);
void Make64Hook(void* source_target, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* target_jmp_name);
uintptr_t ReadLEA32(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset, size_t lea_size, size_t lea_opcode_size, const bool FailedQuiet = false);
uintptr_t ReadLEA32(uintptr_t Address, const wchar_t* Pattern_Name, size_t offset, size_t lea_size, size_t lea_opcode_size);

#define MAKE32HOOK(src,mid,dest,size) Make32to64Hook((void*)(src),(void*)(mid),(void*)(dest),size,TEXT(#src) " Size: " #size ,TEXT(#mid),TEXT(#dest))
#define MAKE32CALL(src,mid,dest,size) Make32to64Call((void*)(src),(void*)(mid),(void*)(dest),size,TEXT(#src) " Size: " #size ,TEXT(#mid),TEXT(#dest))

#define SID(str) ToStringId64(str)
typedef uint64_t StringId64;
#define FNVA1_BASE 0xCBF29CE484222325
// https://github.com/icemesh/StringId/blob/main/StringId64/main.c
constexpr StringId64 ToStringId64(const char* str)
{
    uint64_t base = FNVA1_BASE;
    if (*str)
    {
        do {
            base = 0x100000001B3 * (base ^ *str++);
        } while (*str);
    }
    return base;
}

void SendInputWrapper(WORD inputKey);
void CheckCooldown(bool isBenchmarking, size_t BenchmarkIndex);
void CheckScriptFile();

#pragma GCC diagnostic ignored "-Wunused-function"

static void ShowConsole1()
{
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
}

#pragma GCC diagnostic warning "-Wunused-function"
#define iMSGA(msg, title) MessageBoxA(0, msg, title, MB_ICONINFORMATION)
#define iMSGW(msg, title) MessageBoxW(0, msg, title, MB_ICONINFORMATION)
#define eMSGA(msg, title) MessageBoxA(0, msg, title, MB_ICONERROR)
#define eMSGW(msg, title) MessageBoxW(0, msg, title, MB_ICONERROR)
#define wMSGA(msg, title) MessageBoxA(0, msg, title, MB_ICONWARNING)
#define wMSGW(msg, title) MessageBoxW(0, msg, title, MB_ICONWARNING)

#pragma GCC diagnostic ignored "-Wmicrosoft-string-literal-from-predefined"
// translates to `"foo():1234: "`
// also workarounds intelisense thinking `__FUNCTION__` not exist
#define FUNC_LINE __FUNCSIG__ ":" xstr(__LINE__) ": "
#define LOG(fmt, ...) file_log(L"" FUNC_LINE fmt, __VA_ARGS__)
