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
        LOG(L"Created default config file.");
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
    LOG(L"%s: %s (%i)", wstr(bDebugMenu), GetBoolStr(bDebugMenu) , bDebugMenu);
    LOG(L"%s: %s (%i)", wstr(bShowDebugConsole), GetBoolStr(bShowDebugConsole), bShowDebugConsole);
    LOG(L"%s: (%f)",    wstr(fDebugMenuSize), fDebugMenuSize);
    LOG(L"%s: %s (%i)", wstr(bExtendedDebugMenu), GetBoolStr(bExtendedDebugMenu), bExtendedDebugMenu);
}

void ShowPatchInfo(size_t Hook_Length, size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name)
{
    LOG(L"Patch Name: %s", Patch_Name);
    if (Hook_Length)
        LOG(L"Hook length: %u bytes", Hook_Length);
    LOG(L"Patch length: %llu bytes", Patch_Size);
    LOG(L"Patch address: 0x%016llx", Patch_Addr);
}

uint64_t Memory_PushAllocatorReturnAddr = 0;
void __declspec(naked) Memory_PushAllocator_CC()
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
        mov eax, 0x25
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

#define MEM_TYPE 0x25
// 1.02
// Game addresses
const uint64_t ConsoleOutput_Addr = 0x3bf745;
const uint64_t m_onDisc_DevMenu = 0x3b7692;
const uint64_t Memory_isDebugMemoryAval = 0x3a1584;
const uint64_t Memory_PushAllocatorJMPAddr = 0x3bf648;
const uint64_t DebugDrawStaticContext = 0x4c7fc0;
const uint64_t MaterialDebug_Heap = 0x5090f9;
const uint64_t Rendering_Heap = 0x571a3c;
const uint64_t ParticlesMenu = 0x5ded50;
const uint64_t CreateGameplayMenu_Heap = 0x12aa26b;
const uint64_t UpdateSelectRegionByNameMenu_Heap = 0x2561b8c;
const uint64_t UpdateSelectSpawner = 0x26e4aeb;
const uint64_t MenuHeap_UsableMemorySize = 0x2be15fa; // ALLOCATION_MENU_HEAP
const uint64_t CPUHeap_UsableMemorySize = 0x2be0fdb; // ALLOCATION_CPU_MEMORY
const uint64_t ScriptHeap_UsableMemorySize = 0x2be181a; // ALLOCATION_SCRIPT_HEAP
const uint64_t DevMenu_MenuSize = 0x404e56;
// inline asserts
const uint64_t Memory_ValidateContext = 0x117ABB3;
const uint64_t Assert_UpdateSelectRegionByNameMenu = 0x2561b2f;
const uint64_t Assert_UpdateSelectIgcByNameMenu = 0x25663d0;
const uint64_t Assert_UpdateSelectSpawnerByNameMenu = 0x26e4a97;

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
        const unsigned char mov_ecx_0[] = { 0xb9, 0x00, 0x00, 0x00, 0x00, 0x90 };
        memcpy((void*)bytes_array_4, &fDebugMenuSize, sizeof(bytes_array_4));
        Memory::PatchBytes(base_addr + DevMenu_MenuSize, bytes_array_4, sizeof(bytes_array_4));
        Memory::PatchBytes(base_addr + m_onDisc_DevMenu, mov_ecx_0, sizeof(mov_ecx_0));
        memset(bytes_array_4, 0, sizeof(bytes_array_4));
    }
    if (bShowDebugConsole)
    {
        const unsigned char mov_cl_1_nop[] = { 0xb1, 0x01, 0x90 };
        Memory::PatchBytes(base_addr + ConsoleOutput_Addr, mov_cl_1_nop, sizeof(mov_cl_1_nop));
    }
    if (bExtendedDebugMenu)
    {
        const unsigned char mov_eax_0[] = { 0xb8, 0x00, 0x00, 0x00, 0x00 };
        const unsigned char nop5x[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
        const unsigned char nop1x[] = { 0x90 };
        const unsigned char ret_0[] = { 0x31, 0xc0, 0xc3 };
        const unsigned char ret_1_al[] = { 0xb0, 0x01, 0xc3 };
        const unsigned char heap_type[] = { MEM_TYPE };
        const unsigned char menu_mem_size[] = { 0x84 };
        const unsigned char script_mem_size[] = { 0x80 };
        const unsigned char cpu_mem_size[] = { 0x40 };
        Memory_PushAllocatorReturnAddr = base_addr + Memory_PushAllocatorJMPAddr + 15;
        Memory::DetourFunction64((void*)(base_addr + Memory_PushAllocatorJMPAddr), Memory_PushAllocator_CC, 15);
        Memory::PatchBytes(base_addr + Memory_isDebugMemoryAval, ret_1_al, sizeof(ret_1_al)); // We have debug memory (not really)
        Memory::PatchBytes(base_addr + DebugDrawStaticContext, nop5x, sizeof(nop5x)); // DebugDrawStaticContext
        Memory::PatchBytes(base_addr + MaterialDebug_Heap, heap_type, sizeof(heap_type)); // Material Debug heap override
        Memory::PatchBytes(base_addr + ParticlesMenu, ret_0, sizeof(ret_0)); // Particles menu
        Memory::PatchBytes(base_addr + CreateGameplayMenu_Heap, heap_type, sizeof(heap_type)); // Gameplay
        Memory::PatchBytes(base_addr + UpdateSelectRegionByNameMenu_Heap, heap_type, sizeof(heap_type)); // Select region heap override
        Memory::PatchBytes(base_addr + UpdateSelectSpawner, heap_type, sizeof(heap_type)); // UpdateSelectSpawner heap override
        Memory::PatchBytes(base_addr + MenuHeap_UsableMemorySize, menu_mem_size, sizeof(menu_mem_size)); // Increase memory allocation size
        Memory::PatchBytes(base_addr + ScriptHeap_UsableMemorySize, script_mem_size, sizeof(script_mem_size)); // Script memory
        Memory::PatchBytes(base_addr + CPUHeap_UsableMemorySize, cpu_mem_size, sizeof(cpu_mem_size)); // CPU memory
        // inline asserts
        Memory::PatchBytes(base_addr + Memory_ValidateContext, nop1x, sizeof(nop1x)); // Memory::ValidateContext
        Memory::PatchBytes(base_addr + Assert_UpdateSelectRegionByNameMenu, nop1x, sizeof(nop1x)); // Select region
        Memory::PatchBytes(base_addr + Assert_UpdateSelectIgcByNameMenu, nop1x, sizeof(nop1x)); // Select igc
        Memory::PatchBytes(base_addr + Assert_UpdateSelectSpawnerByNameMenu, nop1x, sizeof(nop1x)); // Select igc
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
    LOG(L"Shutting down " wstr(fp_log) " file handle.");
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
