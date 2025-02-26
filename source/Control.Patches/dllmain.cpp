#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "function_ptr.h"

HMODULE baseModule{};

#define wstr(s) L#s
#define wxstr(s) wstr(s)

// INI Variables
bool bDisableStartupLogo{};
bool bEnableDevMenu{};

void ReadConfig()
{
    inipp::Ini<wchar_t> ini;
    // Initialize config
    std::wstring config_path = L"" PROJECT_NAME ".ini";
    std::wifstream iniFile(config_path);
    if (!iniFile)
    {
        // no ini, lets generate one.
        std::wstring ini_defaults = L"[Settings]\n"
            wstr(bDisableStartupLogo)" = true\n"
            wstr(bEnableDevMenu)" = true\n";
        std::wofstream iniFile(config_path);
        iniFile << ini_defaults;
        bDisableStartupLogo = true;
        bEnableDevMenu = true;
    }
    else
    {
        ini.parse(iniFile);
        inipp::get_value(ini.sections[L"Settings"], wstr(bDisableStartupLogo), bDisableStartupLogo);
        inipp::get_value(ini.sections[L"Settings"], wstr(bEnableDevMenu), bEnableDevMenu);
    }
    LOG(wstr(bDisableStartupLogo) " = %d\n", bDisableStartupLogo);
    LOG(wstr(bEnableDevMenu) " = %d\n", bEnableDevMenu);
}

uintptr_t SkipLogoEnableMissionSelectReturn = 0;

void __attribute__((naked)) SkipLogoEnableMissionSelectAsm()
{
    __asm
    {
        MOV qword ptr[rcx + 0x00000134], 0x1000100;
        movzx edi, byte ptr [bEnableDevMenu];
        mov byte ptr[rcx + 0x00000130], dil;
        movzx edi, byte ptr [bDisableStartupLogo];
        mov byte ptr[rcx + 0x00000139], dil;
        mov byte ptr[rcx + 0x0000013b], dil;
        xor edi, edi;
        mov byte ptr[rcx + 0x0000013c], dil;
        jmp qword ptr[rip + SkipLogoEnableMissionSelectReturn];
    }
}

void** DebugPageControllerPtr = nullptr;

typedef struct DebugPanelVftable_
{
    // void* unk[0x1b];
    void (*Func0)();
    void (*SentMenuEvent)(void* This, void* Event);
    void (*Func2)();
    void (*Func3)();
    void (*Func4)();
    void (*Func5)();
    void (*Func6)();
    void (*Func7)();
    void (*Func8)();
    void (*Func9)();
    void (*Func10)();
    void (*Func11)();
    void (*Func12)();
    void (*Func13)();
    void (*Func14)();
    void (*Func15)();
    void (*Func16)();
    void (*Func17)();
    void (*Func18)();
    void (*Func19)();
    void (*Func20)();
    void (*Func21)();
    void (*Func22)();
    void (*Func23)();
    void (*Func24)();
    void (*Func25)();
    void (*Func26)();
    void (*Update)(void* CurrentPagePtr);
    void (*Func28)();
    void (*Func29)();
    void (*Func30)();
    void (*Func31)();
    void (*Func32)();
    void (*Func33)();
}DebugPanelVftable;

typedef struct DebugPanel_
{
    DebugPanelVftable* Vftable;
}DebugPanel;

typedef struct DebugPanelController_
{
    void* unk;
    DebugPanel** ptr;
}DebugPanelController;

int MenuIndex = 0;

void DebugRenderUpdateInput(void* event_)
{
    DebugPanelController* DebugPanelController1 = (DebugPanelController*)*DebugPageControllerPtr;
    DebugPanel* CurrentController = DebugPanelController1->ptr[MenuIndex];
    CurrentController->Vftable->SentMenuEvent(CurrentController, event_);
}

uiTYPEDEF_FUNCTION_PTR(void, GameClient_UpdateWindowInput_Original, void* this_, void* event_);

void GameClient_UpdateWindowInputHook(void* this_, void* event_)
{
    // use event from gameclient
    DebugRenderUpdateInput(event_);
    GameClient_UpdateWindowInput_Original.ptr(this_, event_);
}

void DebugRenderUpdate2()
{
    if (MenuIndex > 15 || MenuIndex < 0)
    {
        MenuIndex = 0;
    }
    DebugPanelController* DebugPanelController1 = (DebugPanelController*)*DebugPageControllerPtr;
    if (DebugPanelController1)
    {
        DebugPanel* CurrentController = DebugPanelController1->ptr[MenuIndex];
        DebugPanelVftable* CurrentVftable = CurrentController->Vftable;
        CurrentVftable->Update(CurrentController);
    }
}

void __attribute__((naked)) DebugRenderUpdateAsm()
{
    __asm
    {
        // technically this can be anywhere but i didn't want to start an extra thread
        // and sleeping after batching menu draws
        call DebugRenderUpdate2;
        LEA R11, [RSP + 0x90];
        MOV RBX, qword ptr[R11 + 0x48];
        MOVAPS XMM6, xmmword ptr[R11 + -0x10];
        MOVAPS XMM7, xmmword ptr[RSP + 0x70];
        MOVAPS XMM8, xmmword ptr[R11 + -0x30];
        MOVAPS XMM9, xmmword ptr[R11 + -0x40];
        MOVAPS XMM10, xmmword ptr[R11 + -0x50];
        MOV RSP, R11;
        POP R15;
        POP R14;
        POP R13;
        POP R12;
        POP RDI;
        POP RSI;
        POP RBP;
        RET;
    }
}

void ShowConsole()
{
    AllocConsole();
    SetConsoleTitleA(PROJECT_NAME " - Debug Console");
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

void ApplyPatches()
{
    const uintptr_t updateWindowHandler = FindAndPrintPatternW(L"48 89 5c ? 08 57 48 83 ec 20 48 8b d9 48 8b fa 48 8b 0d ? ? ? ? 48 83 c1 08 48 8b 01 ff 50 08", L"updateWindowHandler");
    if (updateWindowHandler)
    {
        const uintptr_t pupdateWindowHandler = (uintptr_t)Memory::u64_Scan(baseModule, updateWindowHandler);
        if (pupdateWindowHandler)
        {
            GameClient_UpdateWindowInput_Original.addr = updateWindowHandler;
            const uintptr_t newF = (uintptr_t)GameClient_UpdateWindowInputHook;
            Memory::PatchBytes(pupdateWindowHandler, &newF, sizeof(newF));
        }
    }
    DebugPageControllerPtr = (void**)ReadLEA32(L"48 89 3d ?? ?? ?? ?? e8 ?? ?? ?? ?? 48 89 45 28", L"DebugPageControllerPtr", 0, 3, 7);
    constexpr uint32_t PtrReturnBytes = 20;
    uintptr_t Ptr = FindAndPrintPatternW(L"48 c7 81 34 01 00 00 00 01 00 01 c6 81 3c 01 00 00 00 33 ff", L"SkipLogoEnableMissionSelect");
    SkipLogoEnableMissionSelectReturn = Ptr + PtrReturnBytes;
    Memory::DetourFunction64((void*)Ptr, (void*)SkipLogoEnableMissionSelectAsm, PtrReturnBytes);
    if (bEnableDevMenu)
    {
        WritePatchPattern_Hook(L"4c 8d 9c 24 90 00 00 00 49 8b 5b 48 41 0f 28 73 f0 0f 28 7c 24 70 45 0f 28 43 d0 45 0f 28 4b c0 45 0f 28 53 b0 49 8b e3 41 5f 41 5e 41 5d 41 5c 5f 5e 5d c3", 52, L"DebugRenderUpdate2", 0, (void*)DebugRenderUpdateAsm, nullptr);
    }
}

DWORD Main()
{
    baseModule = GetModuleHandle(L"Control_DX11.exe") ? : GetModuleHandle(L"Control_DX12.exe");
    if (!baseModule)
    {
        return false;
    }
    ShowConsole();
    ReadConfig();
    ApplyPatches();
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        return Main();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
