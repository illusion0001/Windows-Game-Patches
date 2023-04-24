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

void log_time(void);
void file_log(const wchar_t* fmt, ...);
void file_log2(const wchar_t* fmt, ...);
const wchar_t* GetBoolStr(bool input_bool);
void LoggingInit(const wchar_t* Project_Name, const wchar_t* Project_Log_Name);
void ShowPatchInfo(size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name, uint64_t Patch_Function_Target);
void LogPatchFailed(const wchar_t* Patch_Name, const wchar_t* Patch_Pattern);
uintptr_t WritePatchPattern(const wchar_t* Patch_Pattern, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset);
void WritePatchPattern_Hook(const wchar_t* Patch_Pattern, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, void* Function_Target, uint64_t* Return_Address);
void WritePatchPattern_Int(uint32_t mode, const wchar_t* Patch_Pattern, void* Patch_Value, const wchar_t* Patch_Name, uint64_t Patch_Offset);
void WritePatchAddress(uint64_t Patch_Address, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset);
wchar_t* GetRunningPath(wchar_t* output);
uintptr_t FindAndPrintPatternW(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name);

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

#define LOG(fmt, ...) file_log(L"%-24s:%u " fmt, __FUNCTIONW__, __LINE__, __VA_ARGS__);
