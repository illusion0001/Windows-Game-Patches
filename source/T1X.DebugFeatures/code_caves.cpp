#include "code_caves.hpp"
#include "memory.hpp"

uint64_t Memory_PushAllocatorReturnAddr = 0;
void __attribute__((naked)) Memory_PushAllocator_CC()
{
    __asm 
    {
        // code by infogram
        cmp eax, 0x10; // Debug DMENU
        je retail_memory_type;
        // extra check
        cmp eax, 0xa;  // Debug CPU Memory
        jnz ret_loc;
    retail_memory_type:;
        mov eax, MEM_TYPE;
        mov dword ptr[rcx], eax;
        // original code
    ret_loc:;
        lea rcx, [rsp + 0x30];
        mov dword ptr[rsp + 0x30], eax;
        mov r15, r9;
        mov r14d, r8d;
        jmp[Memory_PushAllocatorReturnAddr];
    }
}

uint64_t Memory_NewHandlerReturnAddr = 0;
void __attribute__((naked)) Memory_NewHandler_CC()
{
    __asm
    {
        mov eax, [rdx];
        // code by infogram
        cmp eax, 0x10; // Debug DMENU
        je retail_memory_type;
        // extra check
        cmp eax, 0xa;  // Debug CPU Memory
        jnz ret_loc;
    retail_memory_type:;
        mov eax, MEM_TYPE;
        mov[rdx], eax;
    ret_loc:;
        mov rbp, rcx;
        lea rcx, [r11 + 0x20];
        mov[rsp + 0x58], eax;
        mov rbx, r8;
        jmp[Memory_NewHandlerReturnAddr];
    }
}

const char* weapon_list_main[] = {"invisible-cube",
                                  "pistol-shotgun-military",
                                  "bloater-pustule",
                                  "pistol-colt-1911-dark",
                                  "bow-joel",
                                  "shiv-d",
                                  "invisible-cube",
                                  "bandage-autofeed",
                                  "rifle-vepr",
                                  "pistol-uber-hunter",
                                  "invisible-cube",
                                  "rifle-m14-sniper",
                                  "bow-ellie",
                                  "invisible-cube",
                                  "invisible-cube",
                                  "bow-ellie",
                                  "pistol-revolver-taurus",
                                  "rifle-m14-sniper",
                                  "bow-scar",
                                  "rifle-mpx5",
                                  "two-hand-axe",
                                  "molotov",
                                  "digital-recorder-carry-t1x",
                                  "club-2h-scar",
                                  "rifle-remington-81",
                                  "hammer-modern",
                                  "pipe-bomb",
                                  "pistol-9mm-firefly",
                                  "gas-mask-lev",
                                  "mp-spotting-scope",
                                  "pistol-revolver-taurus",
                                  "rifle-m14-sniper",
                                  "shotgun-pump-stock",
                                  "pipe-wrench",
                                  "shotgun-remington-pump",
                                  "t1x-shotgun-remington-pump",
                                  "hammer-scar",
                                  "rifle-mini-14-manny",
                                  "flamethrower",
                                  "invisible-cube",
                                  "club-1h-police",
                                  "baseball-bat",
                                  "invisible-cube",
                                  "shiv-b",
                                  "invisible-cube",
                                  "shotgun-pump-firefly",
                                  "pistol-browning",
                                  "rifle-mini-14",
                                  "shotgun-pump-stock",
                                  "baseball-bat-metal",
                                  "bow-lev",
                                  "pistol-shotgun-firefly",
                                  "pistol-uber-firefly",
                                  "m4-rifle-military",
                                  "pickaxe-scar-player",
                                  "ski-aquarium-dog-toy",
                                  "knife-bill-game",
                                  "hatchet-tomahawk",
                                  "short-pipe",
                                  "pistol-revolver-taurus-owen",
                                  "pistol-revolver-scar",
                                  "stun-bomb",
                                  "pistol-ruger",
                                  "rifle-galil",
                                  "rifle-m14-sniper",
                                  "invisible-cube",
                                  "m4-rifle-firefly",
                                  "torch-game",
                                  "invisible-cube",
                                  "bow-ellie",
                                  "molotov",
                                  "machete-wood-joel",
                                  "pistol-xcaliber",
                                  "invisible-cube",
                                  "shiv-f",
                                  "t1x-leather-toolkit",
                                  "pistol-colt-defender",
                                  "shotgun-remington-pump-sawedoff",
                                  "two-hand-axe-scar",
                                  "shotgun-dbl-barrel-scar",
                                  "rifle-vepr",
                                  "rifle-m14-dark",
                                  "rifle-mini-14-wood",
                                  "club-1h-scar",
                                  "bandage-autofeed",
                                  "bow-scar",
                                  "rifle-bolt-firefly",
                                  "pistol-cz75",
                                  "shiv-a",
                                  "pistol-revolver-taurus-owen",
                                  "pistol-9mm-military",
                                  "pistol-beretta",
                                  "invisible-cube",
                                  "invisible-cube",
                                  "invisible-cube",
                                  "fob-basketball",
                                  "rifle-bolt-m24",
                                  "pistol-ruger",
                                  "throw-brick",
                                  "NO_ART_GROUP",
                                  "machete-black",
                                  "bloater-pustule",
                                  "rifle-ruger-scar",
                                  "smoke-bomb",
                                  "machete-scar",
                                  "rifle-winchester-lever",
                                  "shotgun-remington-pump-sawedoff",
                                  "lead-pipe",
                                  "pistol-ruger",
                                  "t1x-shiv-mod-a",
                                  "pistol-shotgun-hunter",
                                  "amp-gift-shop-toy-orca",
                                  "pistol-revolver-firefly",
                                  "molotov",
                                  "smoke-bomb-basic",
                                  "beer-bottle",
                                  "machete-black",
                                  "shotgun-pump-stock-black",
                                  "pipe-bomb",
                                  "crossbow",
                                  "bloater-pustule",
                                  "shotgun-baserri-over-under",
                                  "bloater-pustule",
                                  "pistol-colt-1911-silver",
                                  "pickaxe-scar",
                                  "shotgun-remington-pump",
                                  "rifle-m14-sniper",
                                  "bow-toy",
                                  "shiv-e",
                                  "rifle-sniper-hunter",
                                  "pistol-revolver-357",
                                  "syringe",
                                  "rifle-remington-81",
                                  "scythe-scar",
                                  "hatchet-black",
                                  "crowbar-one-hand",
                                  "hatchet-modern",
                                  "shotgun-remington-pump-black",
                                  "dagger-scar-lev",
                                  "trap-mine",
                                  "invisible-cube",
                                  "rifle-remington-81",
                                  "two-by-four",
                                  "shotgun-dbl-barrel-abby",
                                  "rifle-bolt-m24-dark",
                                  "firecracker",
                                  "oil-can-silencer",
                                  "shotgun-remington-pump",
                                  "rifle-mpx5-tilt",
                                  "short-pipe",
                                  "t1x-turret-sniper",
                                  "t1x-shiv",
                                  "molotov",
                                  "whistle",
                                  "rifle-mpx5",
                                  "dagger-scar",
                                  "NO_ART_GROUP",
                                  "NO_ART_GROUP",
                                  "pistol-glock",
                                  "hatchet-tomahawk",
                                  "machete-wood",
                                  "pistol-cz75-black",
                                  "rifle-bolt-m24",
                                  "pat-snowball",
                                  "pistol-browning-scar",
                                  "sledgehammer",
                                  "fireman-axe",
                                  "shiv-c",
                                  "pistol-revolver-militia",
                                  "pistol-revolver-military",
                                  "t1x-shiv-mod-b",
                                  "pistol-9mm-hunter",
                                  "rifle-semi-auto-firefly",
                                  "rifle-bolt-scar",
                                  "hatchet-scar",
                                  "rifle-remington-bolt",
                                  "tennis-ball-throw",
                                  "t1x-leather-toolkit"};

const char* weapon_list_subsection[] = {"invisible-cube",
                                        "",
                                        "bandage-autofeed",
                                        "bandage-autofeed",
                                        "throw-brick",
                                        "beer-bottle",
                                        "nail-bomb-basic",
                                        "molotov-basic",
                                        "smoke-bomb-basic",
                                        "t1x-mal-gas-can",
                                        "pistol-t1x-beretta-jaguar-pistol",
                                        "t1x-bow-ellie",
                                        "t1x-rifle-bolt-m24",
                                        "rifle-shotgun-pump-hunter",
                                        "pistol-revolver-hunter",
                                        "rifle-bolt-hunter",
                                        "t1x-super-soaker",
                                        "t1x-pistol-9mm",
                                        "t1x-taurus-66b4-revolver",
                                        "t1x-pistol-uber-t1",
                                        "t1x-pistol-shotgun",
                                        "t1x-rifle-bolt",
                                        "t1x-m4-rifle-t1",
                                        "t1x-shotgun-pump-stock",
                                        "t1x-bow-dark",
                                        "t1x-flamethrower",
                                        "t1x-super-soaker",
                                        "t1x-machete-blade",
                                        "t1x-hatchet",
                                        "t1x-baseball-bat",
                                        "t1x-two-by-four",
                                        "t1x-lead-pipe",
                                        "invisible-cube",
                                        "t1x-switchblade"};

const char* weapon_subentry_names[] = {"Misc", "Consumables", "Throwables", "Ellie", "Joel", "Melee"};

const constexpr uint32_t GivePlayerWeapon_ListMax = STRING_SIZEOF(weapon_list_main);
const constexpr uint32_t GivePlayerWeapon_SubListMax = STRING_SIZEOF(weapon_list_subsection);
const constexpr uint32_t GivePlayerWeapon_EntryListMax = STRING_SIZEOF(weapon_subentry_names);

// Main list
uint32_t GivePlayerWeapon_index_count = 0;
uint64_t GivePlayerWeapon_MainReturn = 0;
// Sub entry
uint32_t GivePlayerWeaponSub_index_count = 0;
uint64_t GivePlayerWeapon_SubReturn = 0;
// Entry
uint32_t GivePlayerWeaponEntry_index_count = 0;
uint64_t GivePlayerWeapon_EntryReturn = 0;
uint64_t GivePlayerWeapon_EntryHeaderReturn = 0;
uint32_t GivePlayerWeaponEntryHeader_index_count = 0;

// 1.0.5.0 and above
bool GameVeris1050 = false;

// Game funcs
uint64_t ScriptLookupAddr = 0;

char temp_str[128];
const char* string_index = nullptr;

const char* GivePlayerWeaponMain(const StringId64 Sid, const int32_t mode)
{
    string_index = nullptr;
    switch (mode) {
    case 1: {
        if (GivePlayerWeapon_index_count == GivePlayerWeapon_ListMax)
            GivePlayerWeapon_index_count = 0;
        string_index = weapon_list_main[GivePlayerWeapon_index_count];
        GivePlayerWeapon_index_count++;
        break;
    }
    case 2: {
        if (GivePlayerWeaponSub_index_count == GivePlayerWeapon_SubListMax)
            GivePlayerWeaponSub_index_count = 0;
        string_index = weapon_list_subsection[GivePlayerWeaponSub_index_count];
        GivePlayerWeaponSub_index_count++;
        break;
    }
    case 3: {
        if (GivePlayerWeaponEntry_index_count == GivePlayerWeapon_EntryListMax)
            GivePlayerWeaponEntry_index_count = 0;
        string_index = weapon_subentry_names[GivePlayerWeaponEntry_index_count];
        GivePlayerWeaponEntry_index_count++;
        break;
    }
    default: {
        CLEAR_MEM((void*)temp_str, sizeof(temp_str));
        return temp_str;
    }
    }
    if (string_index[0] == '\0')
        string_index = "(null)";
    _snprintf_s(temp_str, sizeof(temp_str), "%s (#%.16llx)", string_index, Sid);
    return temp_str;
}

const char* GivePlayerWeaponMainSimple(const uint64_t mode)
{
    string_index = nullptr;
    switch (mode) {
    case 1: {
        if (GivePlayerWeapon_index_count == GivePlayerWeapon_ListMax)
            GivePlayerWeapon_index_count = 0;
        string_index = weapon_list_main[GivePlayerWeapon_index_count];
        GivePlayerWeapon_index_count++;
        break;
    }
    case 2: {
        if (GivePlayerWeaponSub_index_count == GivePlayerWeapon_SubListMax)
            GivePlayerWeaponSub_index_count = 0;
        string_index = weapon_list_subsection[GivePlayerWeaponSub_index_count];
        GivePlayerWeaponSub_index_count++;
        break;
    }
    case 3: {
        if (GivePlayerWeaponEntry_index_count == GivePlayerWeapon_EntryListMax)
            GivePlayerWeaponEntry_index_count = 0;
        string_index = weapon_subentry_names[GivePlayerWeaponEntry_index_count];
        GivePlayerWeaponEntry_index_count++;
        break;
    }
    case 4: {
        if (GivePlayerWeaponEntryHeader_index_count == GivePlayerWeapon_EntryListMax)
            GivePlayerWeaponEntryHeader_index_count = 0;
        string_index = weapon_subentry_names[GivePlayerWeaponEntryHeader_index_count];
        GivePlayerWeaponEntryHeader_index_count++;
        break;
    }
    default: {
        CLEAR_MEM((void*)temp_str, sizeof(temp_str));
        return temp_str;
    }
    }
    return string_index;
}

uint64_t temp_reg1 = 0;

void __attribute__((naked)) GivePlayerWeapon_MainCC() {
    __asm {
        cmp byte ptr[GameVeris1050], 1;
        jz case_1050;
        mov rcx, [r15 + 0x8];
        mov rcx, [rsi + rcx * 1];
        mov edx, 1;
        call GivePlayerWeaponMain;
        jmp code_exit;
    case_1050:;
        mov qword ptr[temp_reg1], 0;
        mov rcx, qword ptr[RSI + RCX];
        mov qword ptr[temp_reg1], rcx;
        mov rcx, 1;
        call GivePlayerWeaponMainSimple;
        mov rdx, rax;
        mov byte ptr[rsp + 0x20], 0;
        mov r9, qword ptr[temp_reg1];
    code_exit:;
        jmp[GivePlayerWeapon_MainReturn];
    }
}

void __attribute__((naked)) GivePlayerWeapon_SubCC() {
    __asm {
        cmp byte ptr[GameVeris1050], 1;
        jz case_1050;
        mov rcx, [r13 + 0x8];
        mov rcx, [r12 + rcx * 1];
        mov edx, 2;
        call GivePlayerWeaponMain;
        jmp code_exit;
    case_1050:;
        mov qword ptr[temp_reg1], 0;
        mov rcx, qword ptr[RCX + R13 * 0x1];
        mov qword ptr[temp_reg1], rcx;
        mov rcx, 2;
        call GivePlayerWeaponMainSimple;
        mov rdx, rax;
        mov byte ptr[rsp + 0x20], 0;
        mov r9, qword ptr[temp_reg1];
    code_exit:;
        jmp[GivePlayerWeapon_SubReturn];
    }
}

void __attribute__((naked)) GivePlayerWeapon_EntryCC() {
    __asm {
        cmp byte ptr[GameVeris1050], 1;
        jz case_1050;
        mov rcx, [r14 + 0x8];
        mov rcx, [r15 + rcx * 1];
        mov edx, 3;
        call GivePlayerWeaponMain;
        jmp code_exit;
    case_1050:;
        mov rcx, 3;
        call GivePlayerWeaponMainSimple;
        mov r8, rax;
    code_exit:;
        jmp[GivePlayerWeapon_EntryReturn];
    }
}

void __attribute__((naked)) GivePlayerWeapon_EntryHeaderCC() {
    __asm {
        mov rcx, 4;
        call GivePlayerWeaponMainSimple;
        mov rdx, rax;
        jmp[GivePlayerWeapon_EntryHeaderReturn];
    }
}

char temp_buffer[256];
const char* ScriptPrintWarn_CC(const char* ret, const char* fmt, ...)
{
    CLEAR_MEM((void*)temp_buffer, sizeof(temp_buffer));
    va_list args;
    va_start(args, fmt);
    vsprintf_s(temp_buffer, sizeof(temp_buffer), fmt, args);
    va_end(args);
    ret = temp_buffer;
    printf_s(ret);
    return temp_buffer;
}

uint64_t PlayerPtrAddr = 0;
void __attribute__((naked)) GetPlayerPtrAddr_CC()
{
    __asm
    {
        CMP[RCX + 0x98], EDX;
        CMOVZ RAX, RCX;
        MOV[PlayerPtrAddr],RAX;
        RET;
    }
}

uint64_t ActiveTaskDisplayAddr = 0;
uint64_t ActiveTaskDisplayReturnAddr = 0;

struct TaskManager::ActiveTaskData TaskData{};
bool TestTextPrintV = false;
bool ShowPlayerPos = false;

void __attribute__((naked)) ActiveTaskDisplay_CC()
{
    __asm
    {
        lea    rcx, [TaskData];
        lea    rax, [rbp + 0x3c0];
        mov    r8, r14;
        mov    QWORD PTR[rsp + 0x28], rax;
        mov    QWORD PTR[rsp + 0x20], rsi;
        jmp qword ptr[ActiveTaskDisplayReturnAddr];
    }
}

uint64_t DebugPrint_ReturnAddr = 0;
uint64_t DebugPrint_WindowContext = 0;
uint64_t DebugPrint_OriginalAddr = 0;

INIT_FUNCTION_PTR(TextPrintV);

void MyDebugPrintFunction(uintptr_t window_context)
{
    if (TaskData.formatted_size != 0 && TaskData.text_buffer[0] != 0)
    {
        // TextPrintV(window_context, 50., 620., 0.8, 0.8, 0xc0e0e0e0, "Task callback: 0x%016llx\nBuffer size: %lli\nFormatted size: %lli\nFormatted String: %s\n\n", pointer->unk_callback, pointer->buffer_size, pointer->formatted_size, pointer->text_buffer);
        TextPrintV(window_context, 50., 667., 0.8, 0.8, 0xc0e0e0e0, TaskData.text_buffer);
        memset(&TaskData, 0, sizeof(TaskData));
    }
    if (TestTextPrintV)
    {
        TextPrintV(window_context, 50., 120., 0.8, 0.8, 0xc0e0e0e0, "Hello world!");
    }
    if (ShowPlayerPos && PlayerPtrAddr)
    {
        struct player_cord_struct
        {
            float world_x;
            float world_y;
            float world_z;
        };
        struct player_cord_struct* pointer;
        void* addr = (void*)(PlayerPtrAddr + 0x1b0);
        pointer = (struct player_cord_struct*)addr;
        TextPrintV(window_context, 50., 120., 0.8, 0.8, 0xc0e0e0e0, "Player Position: %.2f %.2f %.2f", pointer->world_x, pointer->world_y, pointer->world_z);
    }
    return;
}

void __attribute__((naked)) DebugPrint_CC()
{
    __asm
    {
        mov qword ptr[DebugPrint_WindowContext], rcx;
        call qword ptr[DebugPrint_OriginalAddr];
        mov rcx, qword ptr[DebugPrint_WindowContext]; // need the window context
        call qword ptr[MyDebugPrintFunction];
        jmp qword ptr[DebugPrint_ReturnAddr];
    }
}
