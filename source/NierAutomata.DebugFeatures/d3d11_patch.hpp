#pragma once

#include <d3d11.h>
#include "function_ptr.h"

HRESULT Present_Hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT ResizeBuffers_Hook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
int CreateD3DDevice_Hook(void* param_1, void* param_2, void* param_3, void* param_4, void* param_5, void* param_6);

TYPEDEF_FUNCTION_PTR(HRESULT, Present_Original, IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
TYPEDEF_FUNCTION_PTR(HRESULT, ResizeBuffers_Original, IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
TYPEDEF_FUNCTION_PTR(int, CreateD3DDevice_Original, void* param_1, void* param_2, void* param_3, void* param_4, void* param_5, void* param_6);
