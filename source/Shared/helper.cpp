#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include <filesystem>

bool bLoggingEnabled;
FILE* fp_log;
std::time_t current_time;
struct tm timeinfo;

void log_time(void)
{
    return;
    wchar_t wtime[256];
    if (localtime_s(&timeinfo, &current_time) == 0 && std::wcsftime(wtime, _countof(wtime), L"%A %c", &timeinfo))
        fwprintf_s(fp_log, L"%-32s ", wtime);
}

void file_log(const wchar_t* fmt, ...)
{
    log_time();
    va_list args;
    va_start(args, fmt);
    if (fp_log)
    {
        vfwprintf_s(fp_log, fmt, args);
    }
    vfwprintf_s(stdout, fmt, args);
    va_end(args);
}

void file_log2(const wchar_t* fmt, ...)
{
    if (!bLoggingEnabled || !fp_log)
        return;
    va_list args;
    va_start(args, fmt);
    vfwprintf_s(fp_log, fmt, args);
    va_end(args);
}

const wchar_t* GetBoolStr(bool input_bool)
{
    return input_bool ? L"true" : L"false";
}

void LoggingInit(const wchar_t* Project_Name, const wchar_t* Project_Log_Name)
{
    errno_t file_stat = _wfopen_s(&fp_log, Project_Log_Name, L"w+, ccs=UTF-8");
    if (file_stat == 0)
    {
        bLoggingEnabled = true;
        std::locale::global(std::locale("en_US.utf8"));
        current_time = std::time(nullptr);
        LOG(L"Log file opened at %s\n", Project_Log_Name);
    }
    else
    {
        bLoggingEnabled = false;
        wchar_t errorText[256] = { 0 };
        wchar_t errorMsg[512] = { 0 };
        _wcserror_s(errorText, _countof(errorText), file_stat);
        _snwprintf_s(errorMsg, _countof(errorMsg), _TRUNCATE, L"Failed to open log file. (%s)\nError code: %i (0x%x) %s", Project_Log_Name, file_stat, file_stat, errorText);
        MessageBox(0, errorMsg, Project_Name, MB_ICONWARNING);
    }
}

void ShowPatchInfo(size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name, uint64_t Patch_Function_Target)
{
    LOG(L"Patch Name: %s\n", Patch_Name);
    LOG(L"Patch length: %llu bytes\n", Patch_Size);
    LOG(L"Patch address: 0x%016llx\n", Patch_Addr);
    if (Patch_Function_Target)
        LOG(L"Patch Function Target: 0x%016llx\n", Patch_Function_Target);
}

void LogPatchFailed(const wchar_t* Patch_Name, const wchar_t* Patch_Pattern, const bool FailedQuiet)
{
    LOG(L"\"%s\" Pattern Scan Failed. Please adjust your scan patterns and try again\n", Patch_Name);
    LOG(L"Pattern \"%s\"\n", Patch_Pattern);
    if (FailedQuiet)
    {
        return;
    }
#ifndef _DEBUG
    wchar_t msg[8 * 1024]{};
    _snwprintf_s(msg, _countof(msg), _TRUNCATE, L""
                 "Pattern scan for \"%s\" failed.\n"
                 "Has game been updated?\n"
                 "Pattern: \"%s\"\n\n"
                 "Do you want to continue? (Not recommended)", Patch_Name, Patch_Pattern);
    // `MB_DEFBUTTON2` to select `"No"` on Open
    switch (MessageBox(0, msg, _PROJECT_NAME, (MB_DEFBUTTON2 | MB_YESNO | MB_ICONERROR)))
    {
        case IDNO:
        {
            ExitProcess(0);
            break;
        }
        case IDYES:
        {
            break;
        }
        default:
        {
            break;
        }
    }
#endif
}

uintptr_t WritePatchPattern(const wchar_t* Patch_Pattern, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, const bool FailedQuiet)
{
    uint8_t* Address_Result = Memory::PatternScanW(baseModule, Patch_Pattern);
    uintptr_t Patch_Address = 0;
    if (Address_Result)
    {
        Patch_Address = (uintptr_t)Address_Result + Patch_Offset;
        Memory::PatchBytes(Patch_Address, Patch_Bytes, Patch_Size);
        ShowPatchInfo(Patch_Size, Patch_Address, Patch_Name, 0);
        return Patch_Address;
    }
    else
    {
        LogPatchFailed(Patch_Name, Patch_Pattern, FailedQuiet);
        return 0;
    }
    return 0;
}

void WritePatchPattern_Hook(const wchar_t* Patch_Pattern, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, void* Function_Target, uint64_t* Return_Address, const bool FailedQuiet)
{
    uint8_t* Address_Result = nullptr;
    Address_Result = Memory::PatternScanW(baseModule, Patch_Pattern);
    uint64_t Patch_Address = 0;
    if (Address_Result)
    {
        Patch_Address = (uintptr_t)Address_Result + Patch_Offset;
        if (Return_Address)
        {
            *Return_Address = Patch_Address + Patch_Size;
            LOG(L"%s Hook Info:\n", Patch_Name);
            LOG(L"Start address: 0x%016llx\n", Address_Result);
            LOG(L"Return address: 0x%016llx\n", *Return_Address);
            LOG(L"Function target address: 0x%016llx\n", uintptr_t(Function_Target));
        }
        else
        {
            LOG(L"%s Hook does not contain a returning address\n", Patch_Name);
            LOG(L"Start address: 0x%016llx\n", Address_Result);
            LOG(L"Function target address: 0x%016llx\n", uintptr_t(Function_Target));
            LOG(L"Please make sure it is intentional\n");
        }
        Memory::DetourFunction64((void*)(Patch_Address), Function_Target, Patch_Size);
    }
    else
    {
        LogPatchFailed(Patch_Name, Patch_Pattern, FailedQuiet);
    }
}

void WritePatchPattern_Int(uint32_t mode, const wchar_t* Patch_Pattern, void* Patch_Value, const wchar_t* Patch_Name, uint64_t Patch_Offset)
{
    switch (mode)
    {
        case 1:
        {
            unsigned char bytes_array[1] = { 0 };
            memcpy((void*)bytes_array, &Patch_Value, sizeof(bytes_array));
            WritePatchPattern(Patch_Pattern, bytes_array, sizeof(bytes_array), Patch_Name, Patch_Offset);
            memset(bytes_array, 0, sizeof(bytes_array));
            break;
        }
        case 2:
        {
            unsigned char bytes_array[2] = { 0 };
            memcpy((void*)bytes_array, &Patch_Value, sizeof(bytes_array));
            WritePatchPattern(Patch_Pattern, bytes_array, sizeof(bytes_array), Patch_Name, Patch_Offset);
            memset(bytes_array, 0, sizeof(bytes_array));
            break;
        }
        case 4:
        {
            unsigned char bytes_array[4] = { 0 };
            memcpy((void*)bytes_array, &Patch_Value, sizeof(bytes_array));
            WritePatchPattern(Patch_Pattern, bytes_array, sizeof(bytes_array), Patch_Name, Patch_Offset);
            memset(bytes_array, 0, sizeof(bytes_array));
            break;
        }
        case 8:
        {
            unsigned char bytes_array[8] = { 0 };
            memcpy((void*)bytes_array, &Patch_Value, sizeof(bytes_array));
            WritePatchPattern(Patch_Pattern, bytes_array, sizeof(bytes_array), Patch_Name, Patch_Offset);
            memset(bytes_array, 0, sizeof(bytes_array));
            break;
        }
    }
}

void WritePatchAddress(uint64_t Patch_Address, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset)
{
    uint64_t Patch_Address_Offset = Patch_Address + Patch_Offset;
    Memory::PatchBytes(Patch_Address_Offset, Patch_Bytes, Patch_Size);
    ShowPatchInfo(Patch_Size, Patch_Address_Offset, Patch_Name, 0);
}

void WritePatchAddressFloat32(uint64_t Patch_Address, const float* Patch_Bytes, const wchar_t* Patch_Name, uint64_t Patch_Offset)
{
    uint64_t Patch_Address_Offset = Patch_Address + Patch_Offset;
    Memory::PatchBytes(Patch_Address_Offset, (const unsigned char*)Patch_Bytes, sizeof(float));
    LOG("Write %f to 0x%llx\n", *Patch_Bytes, Patch_Address_Offset);
    ShowPatchInfo(sizeof(float), Patch_Address_Offset, Patch_Name, 0);
}

void WritePatchAddressFloat64(uint64_t Patch_Address, const double* Patch_Bytes, const wchar_t* Patch_Name, uint64_t Patch_Offset)
{
    uint64_t Patch_Address_Offset = Patch_Address + Patch_Offset;
    Memory::PatchBytes(Patch_Address_Offset, (const unsigned char*)Patch_Bytes, sizeof(double));
    LOG("Write %llf to 0x%llx\n", Patch_Bytes, Patch_Address_Offset);
    ShowPatchInfo(sizeof(double), Patch_Address_Offset, Patch_Name, 0);
}

wchar_t* GetRunningPath(wchar_t* output)
{
    GetModuleFileNameW(nullptr, output, MAX_PATH);
    PathRemoveFileSpecW(output);
    return output;
}

wchar_t* GetModuleName(wchar_t* output)
{
    GetModuleFileNameW(nullptr, output, MAX_PATH);
    output = PathFindFileNameW(output);
    return output;
}

wchar_t* ConvertToWideChar(const char* input)
{
    int length = MultiByteToWideChar(CP_UTF8, 0, input, -1, nullptr, 0);
    if (length == 0)
    {
        return nullptr;
    }
    wchar_t* output = new wchar_t[length];
    MultiByteToWideChar(CP_UTF8, 0, input, -1, output, length);
    return output;
}

uintptr_t FindAndPrintPatternW(const HMODULE Module, const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset, const bool FailedQuiet)
{
    size_t Address_Result = (size_t)Memory::PatternScanW(Module, Patch_Pattern);
    size_t Patch_Address = 0;
    if (Address_Result)
    {
        if (offset)
        {
            Patch_Address = offset + Address_Result;
            LOG(L"%s: 0x%016llx - 0x%llx = 0x%016llx\n", Pattern_Name, Address_Result, offset, Patch_Address);
        }
        else
        {
            Patch_Address = Address_Result;
            LOG(L"%s: 0x%016llx\n", Pattern_Name, Patch_Address);
        }
        return Patch_Address;
    }
    else
    {
        LogPatchFailed(Pattern_Name, Patch_Pattern, FailedQuiet);
    }
    return 0;
}

uintptr_t FindAndPrintPatternW(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset, const bool FailedQuiet)
{
    return FindAndPrintPatternW(baseModule, Patch_Pattern, Pattern_Name, offset, FailedQuiet);
}

#define INT3PATTERN_OFFSET 1

uintptr_t FindInt3Jmp(const HMODULE Module)
{
    return FindAndPrintPatternW(Module, L"C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC", L"Int3Jump + " xstr(INT3PATTERN_OFFSET), INT3PATTERN_OFFSET);
}

uintptr_t FindInt3Jmp()
{
    return FindInt3Jmp(baseModule);
}

#undef INT3PATTERN_OFFSET

void Make32to64Call(void* source_target, void* second_jmp, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* second_jmp_name, const wchar_t* target_jmp_name)
{
    if (!source_target || !second_jmp || !target_jmp || source_size < 5)
    {
        LOG(L"Canoot create jump '%s' from '%s' to '%s'\n", source_name, second_jmp_name, target_jmp_name);
        LOG(L"source_target: 0x%p\n", source_target);
        LOG(L"source_size: %u bytes\n", source_size);
        LOG(L"second_jmp: 0x%p\n", second_jmp);
        LOG(L"target_jmp: 0x%p\n", target_jmp);
        return;
    }
    Memory::CallFunction32((void*)source_target, (void*)second_jmp, source_size);
    LOG(L"Created jump %s (0x%016llx) to %s (0x%016llx)\n", source_name, (uintptr_t)source_target, second_jmp_name, (uintptr_t)second_jmp);
    Memory::DetourFunction64((void*)second_jmp, (void*)target_jmp, 14);
    LOG(L"Created jump %s (0x%016llx) to %s (0x%016llx)\n", second_jmp_name, (uintptr_t)second_jmp, target_jmp_name, (uintptr_t)target_jmp);
}

void Make32to64Hook(void* source_target, void* second_jmp, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* second_jmp_name, const wchar_t* target_jmp_name)
{
    if (!source_target || !second_jmp || !target_jmp || source_size < 5)
    {
        LOG(L"Canoot create jump '%s' from '%s' to '%s'\n", source_name, second_jmp_name, target_jmp_name);
        LOG(L"source_target: 0x%p\n", source_target);
        LOG(L"source_size: %u bytes\n", source_size);
        LOG(L"second_jmp: 0x%p\n", second_jmp);
        LOG(L"target_jmp: 0x%p\n", target_jmp);
        return;
    }
    Memory::DetourFunction32((void*)source_target, (void*)second_jmp, source_size);
    LOG(L"Created jump %s (0x%016llx) to %s (0x%016llx)\n", source_name, (uintptr_t)source_target, second_jmp_name, (uintptr_t)second_jmp);
    Memory::DetourFunction64((void*)second_jmp, (void*)target_jmp, 14);
    LOG(L"Created jump %s (0x%016llx) to %s (0x%016llx)\n", second_jmp_name, (uintptr_t)second_jmp, target_jmp_name, (uintptr_t)target_jmp);
}

void Make32Hook(void* source_target, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* target_jmp_name)
{
    if (!source_target || !target_jmp || source_size < 5)
    {
        LOG(L"Canoot create jump '%s' to '%s'\n", source_name, target_jmp_name);
        LOG(L"source_target: 0x%p\n", source_target);
        LOG(L"source_size: %u bytes\n", source_size);
        LOG(L"target_jmp: 0x%p\n", target_jmp);
        return;
    }
    Memory::DetourFunction32((void*)source_target, (void*)target_jmp, source_size);
    LOG(L"Created jump %s (0x%016llx) to %s (0x%016llx)\n", source_name, (uintptr_t)source_target, target_jmp_name, (uintptr_t)target_jmp);
}

void Make64Hook(void* source_target, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* target_jmp_name)
{
    if (!source_target || !target_jmp || source_size < 14)
    {
        LOG(L"Canoot create jump '%s' to '%s'\n", source_name, target_jmp_name);
        LOG(L"source_target: 0x%p\n", source_target);
        LOG(L"source_size: %u bytes\n", source_size);
        LOG(L"target_jmp: 0x%p\n", target_jmp);
        return;
    }
    Memory::DetourFunction64((void*)source_target, (void*)target_jmp, source_size);
    LOG(L"Created jump %s (0x%016llx) to %s (0x%016llx)\n", source_name, (uintptr_t)source_target, target_jmp_name, (uintptr_t)target_jmp);
}

uintptr_t ReadLEA32(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset, size_t lea_size, size_t lea_opcode_size, const bool FailedQuiet)
{
    uintptr_t Address_Result = (uintptr_t)Memory::PatternScanW(baseModule, Patch_Pattern);
    uintptr_t Patch_Address = 0;
    int32_t lea_offset = 0;
    uintptr_t New_Offset = 0;
    if (Address_Result)
    {
        if (offset)
        {
            Patch_Address = offset + Address_Result;
            lea_offset = *(int32_t*)(lea_size + Address_Result);
            New_Offset = Patch_Address + lea_offset + lea_opcode_size;
        }
        else
        {
            Patch_Address = Address_Result;
            lea_offset = *(int32_t*)(lea_size + Address_Result);
            New_Offset = Patch_Address + lea_offset + lea_opcode_size;
        }
        LOG(L"%s: 0x%016llx -> 0x%016llx\n", Pattern_Name, Address_Result, New_Offset);
        return New_Offset;
    }
    else
    {
        LogPatchFailed(Pattern_Name, Patch_Pattern, FailedQuiet);
    }
    return 0;
}

uintptr_t ReadLEA32(uintptr_t Address, const wchar_t* Pattern_Name, size_t offset, size_t lea_size, size_t lea_opcode_size)
{
    uintptr_t Address_Result = Address;
    uintptr_t Patch_Address = 0;
    int32_t lea_offset = 0;
    uintptr_t New_Offset = 0;
    if (Address_Result)
    {
        if (offset)
        {
            Patch_Address = offset + Address_Result;
            lea_offset = *(int32_t*)(lea_size + Address_Result);
            New_Offset = Patch_Address + lea_offset + lea_opcode_size;
        }
        else
        {
            Patch_Address = Address_Result;
            lea_offset = *(int32_t*)(lea_size + Address_Result);
            New_Offset = Patch_Address + lea_offset + lea_opcode_size;
        }
        LOG(L"%s: 0x%016llx -> 0x%016llx\n", Pattern_Name, Address_Result, New_Offset);
        return New_Offset;
    }
    return 0;
}

void SendInputWrapper(WORD inputKey)
{
#ifdef _DEBUG
    if (IsDebuggerPresent())
    {
        // don't want inputs when debugger active
        return;
    }
#endif
    INPUT inputs[2] {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = inputKey;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = inputKey;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(_countof(inputs), inputs, sizeof(inputs[0]));
}

#define DEFAULT_BENCHMARK_COOLDOWN_PATH "Benchmark_Cooldown.ini"
#define DEFAULT_BENCHMARK_COOLDOWN_TIMER 5
#define DEFAULT_INI "[Settings]\r\n" \
                    "WaitTimerSeconds = " xstr(DEFAULT_BENCHMARK_COOLDOWN_TIMER) "\r\n"

#define str(s) #s
#define xstr(s) str(s)

#if !defined(WINXP)

static void CooldownDefault(const wchar_t* inifile)
{
    HANDLE fd = CreateFile(inifile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fd == INVALID_HANDLE_VALUE)
    {
        return;
    }
    else
    {
        const wchar_t* default_ini = L"" DEFAULT_INI;
        DWORD dwBytesWritten = 0;
        WriteFile(fd,
            default_ini,
            (sizeof(DEFAULT_INI) * sizeof(wchar_t)),
            &dwBytesWritten,
            0);
        CloseHandle(fd);
    }
}

static void CheckCooldown(bool isBenchmarking, size_t BenchmarkIndex)
{
    if (!isBenchmarking)
    {
        return;
    }

    std::filesystem::path currentDir = std::filesystem::current_path();
    std::filesystem::path cfgPath = currentDir / L"" DEFAULT_BENCHMARK_COOLDOWN_PATH;
    const wchar_t* iniFilename = cfgPath.native().c_str();

    BOOL check = GetFileAttributes(iniFilename) != INVALID_FILE_ATTRIBUTES;
    if (!check)
    {
        CooldownDefault(iniFilename);
        wchar_t msg[512]{};
        _snwprintf_s(msg, _countof(msg), L"Cooldown file created at: \"%s\".\n"
            "You may edit it to set cooldown timer in seconds after first benchmark run.", iniFilename);
        MessageBox(0, msg, L"BenchmarkCooldown.Check", MB_ICONINFORMATION | MB_OK);
    }

    int cooldownSeconds = GetPrivateProfileInt(L"Settings", L"WaitTimerSeconds", DEFAULT_BENCHMARK_COOLDOWN_TIMER, iniFilename);
    if (cooldownSeconds > 600)
    {
        MessageBox(0, L"Cooldown timer longer than 10 minutes! Setting timer to 1 minute.", L"BenchmarkCooldown.Check", MB_ICONWARNING | MB_OK);
        cooldownSeconds = 60;
    }
    if (isBenchmarking && BenchmarkIndex > 0 && cooldownSeconds > 0)
    {
        Sleep(cooldownSeconds * 1000);
    }
}

#define CHECK_FILE "C:\\benchmarking\\check.txt"

void CheckScriptFile()
{
    BOOL check = GetFileAttributes(TEXT(CHECK_FILE)) != INVALID_FILE_ATTRIBUTES;
    if (!check)
    {
        HANDLE fd = CreateFile(TEXT(CHECK_FILE), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (fd == INVALID_HANDLE_VALUE)
        {
            return;
        }
        else
        {
            const wchar_t* default_ini = L"Example file created by patch\n";
            DWORD dwBytesWritten = 0;
            WriteFile(fd,
                default_ini,
                (wcslen(default_ini) * sizeof(wchar_t)),
                &dwBytesWritten,
                0);
            CloseHandle(fd);
        }
    }
}
#endif
