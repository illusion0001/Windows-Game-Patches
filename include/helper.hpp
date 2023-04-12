#include "stdafx.h"
#include <stdio.h>

extern HMODULE baseModule;
extern bool bLoggingEnabled;
extern FILE* fp_log;
extern std::time_t current_time;
extern struct tm timeinfo;

void log_time(void);
void file_log(const wchar_t* fmt, ...);
void file_log2(const wchar_t* fmt, ...);
const wchar_t* GetBoolStr(bool input_bool);
void LoggingInit(const wchar_t* Project_Name, const wchar_t* Project_Log_Name);
void ShowPatchInfo(size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name, uint64_t Patch_Function_Target);
void LogPatchFailed(const wchar_t* Patch_Name, const wchar_t* Patch_Pattern);
void WritePatchPattern(const wchar_t* Patch_Pattern, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset);
void WritePatchPattern_Hook(const wchar_t* Patch_Pattern, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, uint64_t Hook_Size, void* Function_Target, uint64_t* Return_Address);
void WritePatchAddress(uint64_t Patch_Address, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset);

#define LOG(fmt, ...) file_log(L"%-24s:%u " fmt, __FUNCTIONW__, __LINE__, __VA_ARGS__);

// TODO: Find out where this came from
namespace Memory
{
    void PatchBytes(uintptr_t address, const unsigned char* pattern, unsigned int numBytes);
    void ReadBytes(const uintptr_t address, void* const buffer, const SIZE_T size);
    uintptr_t ReadMultiLevelPointer(uintptr_t base, const std::vector<uint32_t>& offsets);
    size_t GetHookLength(char* src, size_t minLen);
    bool DetourFunction32(void* src, void* dst, int len);
    void* DetourFunction64(void* pSource, void* pDestination, DWORD dwLen);
    HMODULE GetThisDllHandle();
    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    std::uint8_t* PatternScan(void* module, const char* signature);
    // CSGOSimple's pattern scan
    // https://github.com/OneshotGH/CSGOSimple-master/blob/master/CSGOSimple/helpers/utils.cpp
    // Unicode Version
    std::uint8_t* PatternScanW(void* module, const wchar_t* signature);
    std::string GetVersionString();
    std::vector<int> string_to_ints(const std::string& s, char delimiter);
    std::wstring GetVersionProductName();
}
