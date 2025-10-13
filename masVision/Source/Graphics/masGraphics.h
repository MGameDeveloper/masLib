#pragma once

#include "masCommon.h"

#include <wrl/client.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
using namespace Microsoft::WRL;

struct masD3D11
{
	ComPtr<ID3D11Device>           Device;
	ComPtr<ID3D11DeviceContext>    ImmediateContext;
	ComPtr<IDXGISwapChain1>        SwapChain;
	ComPtr<ID3D11RenderTargetView> RenderTargetView;
};

bool masGraphics_Init(const void* Window, uint32_t Width, uint32_t Height);
void masGraphics_Terminate();
const masD3D11* masGraphics_D3D11();

