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

static void ReadConfig()
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

class DebugPanel
{
public:
    virtual void Func0() = 0;
    virtual void SentMenuEvent(void* Event) = 0;
    virtual void Func2() = 0;
    virtual void Func3() = 0;
    virtual void Func4() = 0;
    virtual void Func5() = 0;
    virtual void Func6() = 0;
    virtual void Func7() = 0;
    virtual void Func8() = 0;
    virtual void Func9() = 0;
    virtual void Func10() = 0;
    virtual void Func11() = 0;
    virtual void Func12() = 0;
    virtual void Func13() = 0;
    virtual void Func14() = 0;
    virtual void Func15() = 0;
    virtual void Func16() = 0;
    virtual void Func17() = 0;
    virtual void Func18() = 0;
    virtual void Func19() = 0;
    virtual void Func20() = 0;
    virtual void Func21() = 0;
    virtual void Func22() = 0;
    virtual void Func23() = 0;
    virtual void Func24() = 0;
    virtual void Func25() = 0;
    virtual void Func26() = 0;
    virtual void Update() = 0;
    virtual void Func28() = 0;
    virtual void Func29() = 0;
    virtual void Func30() = 0;
    virtual void Func31() = 0;
    virtual void Func32() = 0;
    virtual void Func33() = 0;
};

struct DebugPanelController
{
    void* unk;
    DebugPanel** ptr;
};

DebugPanelController** DebugPageControllerPtr = nullptr;
int64_t MenuIndex = 0;

static void DebugRenderUpdateInput(void* event_)
{
    DebugPanel* CurrentController = DebugPageControllerPtr[0]->ptr[MenuIndex];
    CurrentController->SentMenuEvent(event_);
}

uiTYPEDEF_FUNCTION_PTR(void, GameClient_UpdateWindowInput_Original, void* this_, void* event_);

static void GameClient_UpdateWindowInputHook(void* this_, void* event_)
{
    // use event from gameclient
    DebugRenderUpdateInput(event_);
    GameClient_UpdateWindowInput_Original.ptr(this_, event_);
}

static void DebugRenderUpdate2()
{
    DebugPanel* CurrentController = DebugPageControllerPtr[0]->ptr[MenuIndex];
    CurrentController->Update();
}

static void ShowConsole()
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

uiTYPEDEF_FUNCTION_PTR(void, RenderLoop_Original, void* param_1, void* param_2, void* param_3);

static void RenderLoopHook(void* param_1, void* param_2, void* param_3)
{
    DebugRenderUpdate2();
    RenderLoop_Original.ptr(param_1, param_2, param_3);
}

uiTYPEDEF_FUNCTION_PTR(void, GameInfoConstructor_Original, void* param_1);

static void GameInfoConstructorHook(uint8_t* param_1)
{
    GameInfoConstructor_Original.ptr(param_1);
    param_1[0x130] = bEnableDevMenu;
    param_1[0x139] = param_1[0x13b] = bDisableStartupLogo;
}

static void ApplyPatches()
{
    if (bEnableDevMenu || bDisableStartupLogo)
    {
        const uintptr_t GameInfoConstructor = FindAndPrintPatternW(L"48 8d 8c 24 e0 04 00 00 e8 ? ? ? ? 90", L"GameInfoConstructor", 8);
        const uintptr_t int3 = FindInt3Jmp();
        if (GameInfoConstructor && int3)
        {
            GameInfoConstructor_Original.addr = ReadLEA32(GameInfoConstructor, L"GameInfoConstructor_Original", 0, 1, 5);
            MAKE32CALL(GameInfoConstructor, int3, GameInfoConstructorHook, 5);
        }
    }
    if (bEnableDevMenu)
    {
        DebugPageControllerPtr = (DebugPanelController**)ReadLEA32(L"48 89 3d ?? ?? ?? ?? e8 ?? ?? ?? ?? 48 89 45 28", L"DebugPageControllerPtr", 0, 3, 7);
        if (DebugPageControllerPtr)
        {
            const uintptr_t RenderLoop = FindAndPrintPatternW(L"48 8b 8f c8 02 00 00 48 8b 97 60 02 00 00 48 8b 49 08 e8 ? ? ? ?", L"RenderLoop", 18);
            const uintptr_t updateWindowHandler = FindAndPrintPatternW(L"48 89 5c ? 08 57 48 83 ec 20 48 8b d9 48 8b fa 48 8b 0d ? ? ? ? 48 83 c1 08 48 8b 01 ff 50 08", L"updateWindowHandler");
            const uintptr_t int3 = FindInt3Jmp();
            if (RenderLoop && int3 && updateWindowHandler)
            {
                const uintptr_t pupdateWindowHandler = (uintptr_t)Memory::u64_Scan(baseModule, updateWindowHandler);
                if (pupdateWindowHandler)
                {
                    GameClient_UpdateWindowInput_Original.addr = updateWindowHandler;
                    const uintptr_t newF = (uintptr_t)GameClient_UpdateWindowInputHook;
                    Memory::PatchBytes(pupdateWindowHandler, &newF, sizeof(newF));
                }
                RenderLoop_Original.addr = ReadLEA32(RenderLoop, L"RenderLoop_Original", 0, 1, 5);
                MAKE32CALL(RenderLoop, int3, RenderLoopHook, 5);
            }
        }
    }
}

static DWORD Main()
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
