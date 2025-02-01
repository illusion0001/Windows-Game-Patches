#include "d3d11_patch.hpp"
#include "helper.hpp"
#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_win32.h>

#include "ImSubMenu.hpp"

#include "Menu.hpp"

#include "memory.hpp"

#include <Xinput.h>

#pragma comment(lib,"Xinput.lib")

WNDPROC oWndProc;

static ID3D11Device* pDevice = NULL;
static ID3D11DeviceContext* pContext = NULL;
static ID3D11RenderTargetView* mainRenderTargetView;
static HWND window = NULL;

static BOOL opened = false;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (opened && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    {
        ShowCursor(1);
        return true;
    }
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

static void InitImGui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //io.IniFilename = NULL; // no configs
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.FontGlobalScale = 2.25f;
    // TODO: Style instead of using defaults
    ImGui::StyleColorsClassic();
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);
}

// https://github.com/NightFyre/Palworld-Internal/blob/8ad3dede62430ee864d726dcc630dc50f941bb5b/src/Game.cpp#L34
static bool XGetAsyncKeyState(WORD combinationButtons)
{
    static BOOL wasPressed = false;
    XINPUT_STATE state{};
    if (XInputGetState(0, &state) == ERROR_SUCCESS)
    {
        BOOL isPressed = (state.Gamepad.wButtons & combinationButtons) == combinationButtons;
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

HRESULT Present_Hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    static BOOL init = false;
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd{};
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;
            ID3D11Texture2D* pBackBuffer = nullptr;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
            pBackBuffer->Release();
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            InitImGui();
            init = true;
        }
        else
        {
            return Present_Original(pSwapChain, SyncInterval, Flags);
        }
    }

    if (GetAsyncKeyState(VK_HOME) & 1 || (XGetAsyncKeyState(XINPUT_GAMEPAD_LEFT_SHOULDER | XINPUT_GAMEPAD_RIGHT_THUMB)))
    {
        opened = !opened;
        set_paused_flag(opened);
    }

    if (init && opened)
    {
        // Create new frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        set_paused_flag(opened);
        RenderMenu(
            PROJECT_NAME "\n"
            "Built: " __DATE__ " @ " __TIME__
        );

        ImGui::Render();
        pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return Present_Original(pSwapChain, SyncInterval, Flags);
}

static void CleanupRenderTarget()
{
    if (mainRenderTargetView)
    {
        mainRenderTargetView->Release();
        mainRenderTargetView = nullptr;
    }
}

static void CreateRenderTarget(IDXGISwapChain* pSwapChain)
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (pDevice && pBackBuffer)
    {
        pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
        pBackBuffer->Release();
    }
}


HRESULT ResizeBuffers_Hook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    CleanupRenderTarget();
    const HRESULT hr = ResizeBuffers_Original(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    if (SUCCEEDED(hr))
    {
        CreateRenderTarget(pSwapChain);
    }
    return hr;
}

// param_4->0x68->0x18->0x40 (Present)
// param_4->0x68->0x18->0x68 (ResizeBuffers)

struct DeviceChainV
{
    char pad[0x40];
    uintptr_t PresentAddr;
    char pad2[0x20];
    uintptr_t ResizeBuffers;
};

struct DeviceChain
{
    char pad[0x18];
    DeviceChainV** vftble;
};

int CreateD3DDevice_Hook(void* param_1, void* param_2, void* param_3, void* param_4, void* param_5, void* param_6)
{
    int ret = CreateD3DDevice_Original(param_1, param_2, param_3, param_4, param_5, param_6);
    if (ret)
    {
        struct ParamPtr
        {
            char pad[0x68];
            DeviceChain* m_DeviceChainWin10;  // 0 on compat
            DeviceChain* m_DeviceChainCompat; // valid on compat
        };
        ParamPtr* pParam = (ParamPtr*)param_2;
        DeviceChain* pChain = pParam->m_DeviceChainWin10 ? : pParam->m_DeviceChainCompat;
        if (!pParam || !pChain)
        {
            return ret;
        }
        DeviceChainV* pDev = pChain->vftble[0];
        if (pDev->PresentAddr && pDev->PresentAddr != (uintptr_t)Present_Hook)
        {
            Present_Original = (Present_Original_ptr)pDev->PresentAddr;
            const uintptr_t newF = (uintptr_t)Present_Hook;
            Memory::PatchBytes(&pDev->PresentAddr, &newF, sizeof(newF));
        }
        if (pDev->ResizeBuffers && pDev->ResizeBuffers != (uintptr_t)ResizeBuffers_Hook)
        {
            ResizeBuffers_Original = (ResizeBuffers_Original_ptr)pDev->ResizeBuffers;
            const uintptr_t newF = (uintptr_t)ResizeBuffers_Hook;
            Memory::PatchBytes(&pDev->ResizeBuffers, &newF, sizeof(newF));
        }
    }
    return ret;
}
