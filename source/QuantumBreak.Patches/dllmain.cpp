#include "stdafx.h"
#include "helper.hpp"
#include "memory.hpp"
#include "function_ptr.h"

#include <Xinput.h>
#pragma comment(lib,"Xinput.lib")

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
    virtual void UpdateEvent(void* Event) = 0;
    virtual void Func01() = 0;
    virtual void Func02() = 0;
    virtual void Func03() = 0;
    virtual void Func04() = 0;
    virtual void Func05() = 0;
    virtual void Func06() = 0;
    virtual void Func07() = 0;
    virtual void Func08() = 0;
    virtual void Func09() = 0;
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
    virtual void Update() = 0;
    virtual void Func24() = 0;
    virtual void Func25() = 0;
    virtual void Func26() = 0;
    virtual void Func27() = 0;
    virtual void Func28() = 0;
    virtual void Func29() = 0;
    virtual void Func30() = 0;
};

struct DebugPanelController
{
    void* unk;
    DebugPanel** ptr;
};

DebugPanelController** DebugPageControllerPtr = nullptr;
int64_t MenuIndex = 0;
BOOL MenuOpen = false;

namespace input
{
    uiTYPEDEF_FUNCTION_PTR(bool, isMenuOn, void);
    uiTYPEDEF_FUNCTION_PTR(bool, isDebugOn, void);
    uiTYPEDEF_FUNCTION_PTR(void, setMenu, BOOL);
    uiTYPEDEF_FUNCTION_PTR(void, setDebug, BOOL);
}

// https://github.com/NightFyre/Palworld-Internal/blob/8ad3dede62430ee864d726dcc630dc50f941bb5b/src/Game.cpp#L34
static bool XGetAsyncKeyState(WORD combinationButtons)
{
    static bool wasPressed = false;
    XINPUT_STATE state{};
    if (XInputGetState(0, &state) == ERROR_SUCCESS)
    {
        bool isPressed = (state.Gamepad.wButtons & combinationButtons) == combinationButtons;
        if (isPressed && !wasPressed)
        {
            wasPressed = true;
            return true;
        }
        else if (!isPressed)
        {
            wasPressed = false;
        }
    }
    return false;
}

static bool XGetAsyncKeyStateDown(WORD combinationButtons)
{
    XINPUT_STATE state{};
    if (XInputGetState(0, &state) == ERROR_SUCCESS)
    {
        return (state.Gamepad.wButtons & combinationButtons) == combinationButtons;
    }
    return false;
}

static void DebugRenderUpdateIndex()
{
    if (MenuOpen)
    {
        bool checkMenu = false;
        if (GetAsyncKeyState(VK_PRIOR) & 1 || (XGetAsyncKeyState(XINPUT_GAMEPAD_RIGHT_SHOULDER)))
        {
            MenuIndex++;
            checkMenu = true;
        }
        else if (GetAsyncKeyState(VK_NEXT) & 1 || (XGetAsyncKeyState(XINPUT_GAMEPAD_LEFT_SHOULDER)))
        {
            MenuIndex--;
            checkMenu = true;
        }
        if (checkMenu)
        {
            constexpr int64_t maxMenu = 15;
            if (MenuIndex > maxMenu)
            {
                MenuIndex = 0;
            }
            else if (MenuIndex < 0)
            {
                MenuIndex = maxMenu;
            }
        }
    }
}

static void DebugRenderUpdateInput(void* event_)
{
    bool MenuStatus = false;
    const WORD button1 = XINPUT_GAMEPAD_LEFT_THUMB;
    const WORD button2 = XINPUT_GAMEPAD_RIGHT_THUMB;
    if (GetAsyncKeyState(VK_HOME) & 1 || (XGetAsyncKeyState(button1 | button2)))
    {
        MenuOpen = !MenuOpen;
        MenuStatus = true;
    }
    DebugRenderUpdateIndex();
    if (MenuStatus)
    {
        static bool set = false;
        static BOOL oldMenu = false, oldDebug = false;
        if (!set)
        {
            oldMenu = input::isMenuOn.ptr();
            oldDebug = input::isDebugOn.ptr();
            input::setMenu.ptr(MenuOpen);
            input::setDebug.ptr(MenuOpen);
            set = true;
        }
        else if (set)
        {
            input::setMenu.ptr(oldMenu);
            input::setDebug.ptr(oldDebug);
            set = false;
        }
    }
    if (MenuOpen && DebugPageControllerPtr[0])
    {
        DebugPanel* CurrentController = DebugPageControllerPtr[0]->ptr[MenuIndex];
        CurrentController->UpdateEvent(event_);
    }
}

static bool DebugCameraCheckButton(void* pInputInstance, int DigitalEnum, bool param_3)
{
    return XGetAsyncKeyState(XINPUT_GAMEPAD_LEFT_THUMB | XINPUT_GAMEPAD_Y);
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
    if (MenuOpen)
    {
        DebugRenderUpdate2();
    }
    RenderLoop_Original.ptr(param_1, param_2, param_3);
}

uiTYPEDEF_FUNCTION_PTR(void, GameInfoConstructor_Original, void* param_1);

namespace r
{
    namespace PluginManager
    {
        struct ExportedFunction
        {
            uint64_t FuncOrd;
            const char* FuncName;
            uint64_t unk1;
            const char* FuncRet;
            uintptr_t FuncPtr;
            uint64_t unk2;
            uint64_t unk3;
            uint64_t unk4;
        };
        uiTYPEDEF_FUNCTION_PTR(void, exportFunction_Original, const void* f);
        static void exportFunction_Hook(ExportedFunction* f)
        {
            printf_s("Export: %s %s @ 0x%llx\n", f->FuncRet ? f->FuncRet : "?", f->FuncName, f->FuncPtr);
            switch (SID(f->FuncName))
            {
                case SID("input::setMenu"):
                {
                    input::setMenu.addr = f->FuncPtr;
                    break;
                }
                case SID("input::setDebug"):
                {
                    input::setDebug.addr = f->FuncPtr;
                    break;
                }
                case SID("input::isMenuOn"):
                {
                    input::isMenuOn.addr = f->FuncPtr;
                    break;
                }
                case SID("input::isDebugOn"):
                {
                    input::isDebugOn.addr = f->FuncPtr;
                    break;
                }
                default:
                {
                    break;
                }
            }
            exportFunction_Original.ptr(f);
        }
    }
}

static void ApplyPatches()
{
    if (bDisableStartupLogo)
    {
    }
    if (bEnableDevMenu)
    {
        const uintptr_t DebugPtr = FindAndPrintPatternW(L"41 88 7e 34 49 89 7e 38 4c 89 35 ? ? ? ?", L"DebugPageControllerPtrPtr", 8);
        if (DebugPtr)
        {
            DebugPageControllerPtr = (DebugPanelController**)ReadLEA32(DebugPtr, L"DebugPageControllerPtr", 0, 3, 7);
        }
        if (DebugPageControllerPtr)
        {
            const uintptr_t RenderLoop = FindAndPrintPatternW(L"48 8b 4b 60 48 8b 01 ff 50 48 48 8b 4b 68 e8 ? ? ? ?", L"RenderLoop", 14);
            const uintptr_t updateWindowHandler = FindAndPrintPatternW(L"48 89 5c 24 ? 57 48 83 ec ? 48 8b f9 48 8b da 48 8b ca e8 ? ? ? ?", L"updateWindowHandler");
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
        const HMODULE rmdlibModule = GetModuleHandle(L"rl_x64_f.dll");
        if (rmdlibModule)
        {
            const uintptr_t funcptr = (uintptr_t)GetProcAddress(rmdlibModule, "?exportFunction@PluginManager@r@@SAXPEAUExportedFunction@2@@Z");
            if (funcptr)
            {
                const uintptr_t pFunc = (uintptr_t)Memory::u64_Scan(baseModule, funcptr);
                if (pFunc)
                {
                    const uintptr_t newF = (uintptr_t)r::PluginManager::exportFunction_Hook;
                    r::PluginManager::exportFunction_Original.addr = funcptr;
                    Memory::PatchBytes(pFunc, &newF, sizeof(uintptr_t));
                }
            }
        }
    }
}

static DWORD Main()
{
    baseModule = GetModuleHandle(L"QuantumBreak.exe");
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
