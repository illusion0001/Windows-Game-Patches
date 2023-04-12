#include "stdafx.h"
#include "helper.hpp"

HMODULE baseModule = GetModuleHandle(NULL);
#define wstr(s) L#s
#define wxstr(s) wstr(s)
#define _PROJECT_NAME L"T1X.DebugFeatures"
#define _PROJECT_LOG_PATH _PROJECT_NAME L".log"

FILE* fp_log;
std::time_t current_time;
struct tm timeinfo;

// INI Variables
bool bLoggingEnabled;
bool bDebugMenu;
bool bShowDebugConsole;
float fDebugMenuSize;
bool bExtendedDebugMenu;

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

#define LOG(fmt, ...) file_log(L"%-24s:%u " fmt, __FUNCTIONW__, __LINE__, __VA_ARGS__);

void LoggingInit(void)
{
    errno_t file_stat = _wfopen_s(&fp_log, _PROJECT_LOG_PATH, L"w+, ccs=UTF-8");
    if (file_stat == 0)
    {
        bLoggingEnabled = true;
        std::locale::global(std::locale("en_US.utf8"));
        current_time = std::time(nullptr);
        LOG(L"Log file opened at " _PROJECT_LOG_PATH "\n");
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

    LOG(_PROJECT_NAME " Built: " __TIME__ " @ " __DATE__ "\n");
    LOG(L"Game Name: %s\n", wsGameName.c_str());
    LOG(L"Game Path: %s\n", exePathWString.c_str());

    // Initialize config
    std::wstring config_path = _PROJECT_NAME L".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        LOG(L"Failed to load config file.\n");
        std::wstring ini_defaults = L"[Settings]\n"
                                    wstr(bDebugMenu)" = true\n"
                                    wstr(bShowDebugConsole)" = false\n"
                                    wstr(fDebugMenuSize)" = 0.6\n"
                                    "; Extended Debug Menu by infogram\n"
                                    wstr(bExtendedDebugMenu)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDebugMenu = true;
        bShowDebugConsole = false;
        bExtendedDebugMenu = true;
        fDebugMenuSize = 0.6;
        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDebugMenu), bDebugMenu);
        inipp::get_value(ini.sections[L"Settings"], wstr(bShowDebugConsole), bShowDebugConsole);
        inipp::get_value(ini.sections[L"Settings"], wstr(bExtendedDebugMenu), bExtendedDebugMenu);
        inipp::get_value(ini.sections[L"Settings"], wstr(fDebugMenuSize), fDebugMenuSize);
    }

    // Log config parse
    LOG(L"%s: %s (%i)\n", wstr(bDebugMenu), GetBoolStr(bDebugMenu) , bDebugMenu);
    LOG(L"%s: %s (%i)\n", wstr(bShowDebugConsole), GetBoolStr(bShowDebugConsole), bShowDebugConsole);
    LOG(L"%s: (%f)\n",    wstr(fDebugMenuSize), fDebugMenuSize);
    LOG(L"%s: %s (%i)\n", wstr(bExtendedDebugMenu), GetBoolStr(bExtendedDebugMenu), bExtendedDebugMenu);
}

void ShowPatchInfo(size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name, uint64_t Patch_Function_Target)
{
    LOG(L"Patch Name: %s\n", Patch_Name);
    LOG(L"Patch length: %llu bytes\n", Patch_Size);
    LOG(L"Patch address: 0x%016llx\n", Patch_Addr);
    if (Patch_Function_Target)
        LOG(L"Patch Function Target: 0x%016llx\n", Patch_Function_Target);
}

#define MEM_TYPE 0x25 // TODO: Check this heap type

uint64_t Memory_PushAllocatorReturnAddr = 0;
void __attribute__((naked)) Memory_PushAllocator_CC()
{
    __asm
    {
        // code by infogram
        cmp eax, 0x10
        je retail_memory_type
        // extra check
        cmp eax, 0xa
        jnz ret_loc
        retail_memory_type :
        mov eax, MEM_TYPE
        mov dword ptr [rcx], eax
        // original code
        ret_loc :
        lea rcx, [rsp + 0x30]
        mov dword ptr[rsp + 0x30], eax
        mov r15, r9
        mov r14d, r8d
        jmp[Memory_PushAllocatorReturnAddr]
    }
}

const wchar_t* DevMenu_MenuSize = L"c7 41 ?? 9a 99 19 3f 49 8b d1 c6 41 ?? 00 44 89 51 ??";
const wchar_t* ConsoleOutput = L"0f 45 ce 88 0d ?? ?? ?? ?? 48 8d ?? ?? ?? ?? ?? e8 ?? ?? ?? ??";
const wchar_t* m_onDisc_DevMenu = L"8a 8f ?? ?? ?? ?? 84 c9 0f 94 c2 84 c9 0f 95 c1";
// thanks to infogram for offsets
const wchar_t* Memory_isDebugMemoryAval = L"32 c0 c3 cc 4c 8b dc 49 89 5b 10 49 89 6b 18 49 89 73 20 57 41 56 41 57 48 83 ec 30 48 8d 69 30 45 8b f0 4c 8b fa 48 8d 05 ?? ?? ?? ??";
const wchar_t* Memory_PushAllocatorJMPAddr = L"48 8d 4c 24 30 89 44 24 30 4d 8b f9 45 8b f0 48 8b ea e8 ?? ?? ?? ??";
const wchar_t* DebugDrawStaticContext = L"e8 ?? ?? ?? ?? 33 d2 33 c9 e8 ?? ?? ?? ?? 80 b8 3d 3f 00 00 00 75 ??";
const wchar_t* MaterialDebug_Heap = L"c7 44 24 ?? 10 00 00 00 b9 18 00 00 00 48 8d 54 24 ?? e8 ?? ?? ?? ??";
const wchar_t* ParticlesMenu = L"40 56 48 83 ec 30 48 8b f1 33 c9 e8 ?? ?? ?? ?? 84 c0 0f 84 ?? ?? ?? ?? 48 89 5c 24 40 4c 8d 0d ?? ?? ?? ??";
const wchar_t* UpdateSelectRegionByNameMenu_Heap = L"b9 08 40 02 00 c7 44 24 ?? 0a 00 00 00 48 8d 54 24 ??";
const wchar_t* UpdateSelectSpawner = L"c7 44 24 ?? 0a 00 00 00 48 8d 54 24 ?? 88 5c 24 ?? b9 08 20 04 00 e8 ?? ?? ?? ??";
const wchar_t* Memory_ValidateContext = L"cc 48 85 db 0f 84 ?? ?? ?? ?? 33 c9 ff d3 90 e9 ?? ?? ?? ?? 48 8b 43 08 49 39 47 08 75 ?? 48 8b 43 10 49 39 47 10 0f 84 ?? ?? ?? ??";
const wchar_t* MenuHeap_UsableMemorySize = L"80 00 00 00 05 00 00 00 00 00 10 00 00 00 00 00 00 40 00 00"; // ALLOCATION_MENU_HEAP
const wchar_t* ScriptHeap_UsableMemorySize = L"5e 00 00 00 05 00 00 00 00 00 12 00 00 00 00 00 00 40 00 00"; // ALLOCATION_SCRIPT_HEAP
const wchar_t* CPUHeap_UsableMemorySize = L"03 00 00 00 02 00 00 00 00 00 e0 33 00 00 00 00 00 00 10 00"; // ALLOCATION_CPU_MEMORY
// inline asserts
// based on source lines
const wchar_t* Assert_UpdateSelectRegionByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 8e 00 00 00";
const wchar_t* Assert_UpdateSelectIgcByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? bb 10 00 00 00 41 b9 84 03 00 00";
const wchar_t* Assert_UpdateSelectSpawnerByNameMenu = L"cc 48 85 db 74 ?? 33 c9 ff d3 48 8d 05 ?? ?? ?? ?? c7 44 24 ?? 10 00 00 00 41 b9 a3 00 00 00";

void LogPatchFailed(const wchar_t* Patch_Name, const wchar_t* Patch_Pattern)
{
    LOG(L"%s Pattern Scan Failed. Please adjust your scan patterns and try again\n", Patch_Name);
    LOG(L"Pattern %s\n", Patch_Pattern);
}

void WritePatchPattern(const wchar_t* Patch_Pattern, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset)
{
    uint8_t* Address_Result = Memory::PatternScanW(baseModule, Patch_Pattern);
    uint64_t Patch_Address = 0;
    if (Address_Result)
    {
        Patch_Address = (uintptr_t)Address_Result + Patch_Offset;
        Memory::PatchBytes(Patch_Address, Patch_Bytes, Patch_Size);
        ShowPatchInfo(Patch_Size, Patch_Address, Patch_Name, 0);
    }
    else
    {
        LogPatchFailed(Patch_Name, Patch_Pattern);
    }
}

void WritePatchPattern_Hook(const wchar_t* Patch_Pattern, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, uint64_t Hook_Size, void* Function_Target, uint64_t* Return_Address)
{
    uint8_t* Address_Result = nullptr;
    Address_Result = Memory::PatternScanW(baseModule, Patch_Pattern);
    uint64_t Patch_Address = 0;
    if (Address_Result)
    {
        Patch_Address = (uintptr_t)Address_Result + Patch_Offset;
        *Return_Address = Patch_Address + Hook_Size;
        Memory::DetourFunction64((void*)(Patch_Address), Function_Target, Hook_Size);
        ShowPatchInfo(Patch_Size, Patch_Address, Patch_Name, uintptr_t(Function_Target));
    }
    else
    {
        LogPatchFailed(Patch_Name, Patch_Pattern);
    }
}

void ApplyDebugPatches(void)
{
    uint64_t base_addr = (uintptr_t)(baseModule);
    if (bDebugMenu)
    {
        if (fDebugMenuSize > 1)
            fDebugMenuSize = 1.0;
        else if (fDebugMenuSize < 0.4)
            fDebugMenuSize = 0.4;
        unsigned char bytes_array_4[4] = { 0 };
        memcpy((void*)bytes_array_4, &fDebugMenuSize, sizeof(bytes_array_4));
        WritePatchPattern(DevMenu_MenuSize, bytes_array_4, sizeof(bytes_array_4), wstr(fDebugMenuSize), 3);
        memset(bytes_array_4, 0, sizeof(bytes_array_4));
        const unsigned char mov_ecx_0[] = { 0xb9, 0x00, 0x00, 0x00, 0x00, 0x90 };
        WritePatchPattern(m_onDisc_DevMenu, mov_ecx_0, sizeof(mov_ecx_0), wstr(m_onDisc_DevMenu), 0);
    }
    if (bShowDebugConsole)
    {
        const unsigned char mov_cl_1_nop[] = { 0xb1, 0x01, 0x90 };
        WritePatchPattern(ConsoleOutput, mov_cl_1_nop, sizeof(mov_cl_1_nop), wstr(ConsoleOutput), 0);
    }
    if (bExtendedDebugMenu)
    {
        const unsigned char nop5x[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
        const unsigned char ret_1_al[] = { 0xb0, 0x01, 0xc3 };
        const unsigned char nop1x[] = { 0x90 };
        const unsigned char ret_0[] = { 0x31, 0xc0, 0xc3 };
        const unsigned char heap_type[] = { MEM_TYPE };
        const unsigned char menu_mem_size[] = { 0x84 };
        const unsigned char script_mem_size[] = { 0x80 };
        const unsigned char cpu_mem_size[] = { 0x40 };
        WritePatchPattern_Hook(Memory_PushAllocatorJMPAddr, 15, wstr(Memory_PushAllocatorJMPAddr), 0, 15, &Memory_PushAllocator_CC, &Memory_PushAllocatorReturnAddr);
        WritePatchPattern(Memory_isDebugMemoryAval, ret_1_al, sizeof(ret_1_al), wstr(Memory_isDebugMemoryAval), 0);
        WritePatchPattern(DebugDrawStaticContext, nop5x, sizeof(nop5x), wstr(DebugDrawStaticContext), 0);
        WritePatchPattern(MaterialDebug_Heap, heap_type, sizeof(heap_type), wstr(MaterialDebug_Heap), 4);
        WritePatchPattern(ParticlesMenu, ret_0, sizeof(ret_0), wstr(ParticlesMenu), 0);
        WritePatchPattern(UpdateSelectRegionByNameMenu_Heap, heap_type, sizeof(heap_type), wstr(UpdateSelectRegionByNameMenu_Heap), 9);
        WritePatchPattern(UpdateSelectSpawner, heap_type, sizeof(heap_type), wstr(UpdateSelectSpawner), 4);
        WritePatchPattern(MenuHeap_UsableMemorySize, menu_mem_size, sizeof(menu_mem_size), wstr(MenuHeap_UsableMemorySize), 10);
        WritePatchPattern(ScriptHeap_UsableMemorySize, script_mem_size, sizeof(script_mem_size), wstr(ScriptHeap_UsableMemorySize), 10);
        WritePatchPattern(CPUHeap_UsableMemorySize, cpu_mem_size, sizeof(cpu_mem_size), wstr(CPUHeap_UsableMemorySize), 11);
        WritePatchPattern(Memory_ValidateContext, nop1x, sizeof(nop1x), wstr(Memory_ValidateContext), 0);
        WritePatchPattern(Assert_UpdateSelectRegionByNameMenu, nop1x, sizeof(nop1x), wstr(Assert_UpdateSelectRegionByNameMenu), 0);
        WritePatchPattern(Assert_UpdateSelectIgcByNameMenu, nop1x, sizeof(nop1x), wstr(Assert_UpdateSelectIgcByNameMenu), 0);
        WritePatchPattern(Assert_UpdateSelectSpawnerByNameMenu, nop1x, sizeof(nop1x), wstr(Assert_UpdateSelectSpawnerByNameMenu), 0);
    }
}

void __stdcall Main()
{
    bLoggingEnabled = false;
    bDebugMenu = false;
    bShowDebugConsole = false;
    bExtendedDebugMenu = false;
    fDebugMenuSize = 0.6;
    LoggingInit();
    ReadConfig();
    ApplyDebugPatches();
    LOG(L"Shutting down " wstr(fp_log) " file handle.\n");
    fclose(fp_log);
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
        Main();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}