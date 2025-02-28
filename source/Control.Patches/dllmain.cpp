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
    virtual void Update() = 0;  // Retail Version
    virtual void Update2() = 0; // HDR Version has a new method
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
BOOL MenuOpen = false;

namespace input
{
    namespace Gamepad
    {
        uiTYPEDEF_FUNCTION_PTR(bool, buttonPressed, void* pGamepad, int gamepadKey);
    }
    namespace inputX86
    {
        uiTYPEDEF_FUNCTION_PTR(void*, isAnyKeyPressed, void* pInputManagerInstance, int32_t* out);
    }
    namespace InputManager
    {
        struct Instance
        {
            void* pad1[6];
            void* kb;
            char pad2[0x3c];
            bool isMenuOn;
            char pad3[0x5];
            bool isMenuControlsOn;
            char pad4[0x2];
            bool isPlayerControl1;
            bool isPlayerControl2;
        };

        static_assert(__builtin_offsetof(Instance, kb) == 0x30, "");
        static_assert(__builtin_offsetof(Instance, isMenuOn) == 0x74, "");
        static_assert(__builtin_offsetof(Instance, isMenuControlsOn) == 0x7a, "");
        static_assert(__builtin_offsetof(Instance, isPlayerControl1) == 0x7d, "");
        static_assert(__builtin_offsetof(Instance, isPlayerControl2) == 0x7e, "");

        Instance** pInstance = nullptr;

        uiTYPEDEF_FUNCTION_PTR(void*, getKeyboard, void* pInputManagerInstance);
        uiTYPEDEF_FUNCTION_PTR(void*, getGamepad, void* pInputManagerInstance, int gamepadIdx);
        uiTYPEDEF_FUNCTION_PTR(void*, readDigital, void* pInputManagerInstance, int key);
        uiTYPEDEF_FUNCTION_PTR(void*, readKeyboard, void* pInputManagerInstance, int key);
    }
    namespace Keyboard
    {
        uiTYPEDEF_FUNCTION_PTR(bool, keyPressed, void* kb, int kn);
    }
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
            constexpr int64_t maxMenu = 14;
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

struct StateMachineState
{
    char pad[0x269];
    bool m_RequestPause;
    bool m_FreezeFrame;
    bool m_Unpause;
    bool m_StepOneFrame;
};

struct GameClient
{
    void* pad[0x68 / 0x8];
    StateMachineState* pStateMachineState;
};

struct GameHelper
{
    void* pad[6];
    GameClient pGameClientBase;
};

GameHelper** s_pGameClientBase = 0;

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
    input::InputManager::Instance* p = input::InputManager::pInstance[0];
    if (p)
    {
        p->isPlayerControl1 = p->isPlayerControl2 = !XGetAsyncKeyStateDown(button1);
        if (MenuStatus)
        {
            static bool set = false;
            if (!set)
            {
                set = MenuOpen;
            }
            else if (set)
            {
                set = MenuOpen;
            }
            p->isMenuOn = p->isMenuControlsOn = MenuOpen;
        }
    }
    if (!MenuOpen)
    {
        return;
    }
    DebugPanel* CurrentController = DebugPageControllerPtr[0]->ptr[MenuIndex];
    CurrentController->SentMenuEvent(event_);
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

static bool isHDRVer = false;

static void DebugRenderUpdate2()
{
    DebugPanel* CurrentController = DebugPageControllerPtr[0]->ptr[MenuIndex];
    isHDRVer ? CurrentController->Update2() : CurrentController->Update();
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
    if (!MenuOpen)
    {
        return;
    }
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
        const HMODULE inputModule = GetModuleHandle(L"input_rmdwin7_f.dll") ? : GetModuleHandle(L"input_rmdwin10_f.dll");
        {
            const HMODULE AppModule = GetModuleHandle(L"app_rmdwin7_f.dll") ? : GetModuleHandle(L"app_rmdwin10_f.dll");
            if (AppModule)
            {
                isHDRVer = GetProcAddress(AppModule, "?handleAppEvent@EventHandler@app@@UEAA_NPEAUDisplayChangedEvent@2@@Z") ? true : false;
            }
        }
        if (inputModule)
        {
            input::InputManager::pInstance = (input::InputManager::Instance**)GetProcAddress(inputModule, "?sm_pInstance@InputManager@input@@0PEAV12@EA");
            input::Gamepad::buttonPressed.addr = (uintptr_t)GetProcAddress(inputModule, "?buttonPressed@Gamepad@input@@QEBA_NH@Z");
            input::InputManager::getGamepad.addr = (uintptr_t)GetProcAddress(inputModule, "?getGamepad@InputManager@input@@QEAAPEAVGamepad@2@H@Z");
            input::Keyboard::keyPressed.addr = (uintptr_t)GetProcAddress(inputModule, "?keyPressed@Keyboard@input@@QEBA_NH@Z");
            input::InputManager::getKeyboard.addr = (uintptr_t)GetProcAddress(inputModule, "?getKeyboard@InputManager@input@@QEAAPEAVKeyboard@2@XZ");
            input::InputManager::readDigital.addr = (uintptr_t)GetProcAddress(inputModule, "?readDigital@InputManager@input@@QEAA_NH_N@Z");
            input::InputManager::readKeyboard.addr = (uintptr_t)GetProcAddress(inputModule, "?readKeyboard@InputManager@input@@AEAA_NHW4ControlType@2@@Z");
            input::inputX86::isAnyKeyPressed.addr = (uintptr_t)GetProcAddress(inputModule, "?isAnyKeyPressed@InputX86@input@@QEAA_NAEAH@Z");
        }
        const HMODULE coreGameModule = GetModuleHandle(L"coregame_rmdwin7_f.dll") ? : GetModuleHandle(L"coregame_rmdwin10_f.dll");
        if (coreGameModule)
        {
            s_pGameClientBase = (GameHelper**)GetProcAddress(coreGameModule, "?s_pGameClientBase@GameHelper@coregame@@2PEAVGameClientBase@2@EA");
        }
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
        const uintptr_t DebugCameraKeyCheckAddr = FindAndPrintPatternW(L"48 8b 08 41 8d 50 2c ff 15 ? ? ? ?", L"DebugCameraKeyCheck", 7);
        if (DebugCameraKeyCheckAddr)
        {
            const uintptr_t int3 = FindInt3Jmp();
            if (int3)
            {
                MAKE32CALL(DebugCameraKeyCheckAddr, int3, DebugCameraCheckButton, 6);
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
