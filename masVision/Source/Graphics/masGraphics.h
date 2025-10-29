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


void masGraphicsUI_Prepare();
void masGraphicsUI_Render();
void masGraphics_Present();

ID3D11Buffer*             masGraphics_CreateVertexBuffer(const void* Vertices, uint32_t VertexCount, uint64_t VertexSize);
ID3D11Buffer*             masGraphics_CreateIndexBuffer(const uint32_t* Indices, uint32_t IndexCount);
//ID3D11Texture2D*          masGraphics_CreateTexture2D(const char* Path);
ID3D11ShaderResourceView* masGraphics_CreateShaderResourceView_Texture2D(ID3D11Texture2D* pTexture2D);
ID3D11SamplerState*       masGraphics_CreateSamplerState();

ID3D11Texture2D* masGraphics_CreateTexture2D(uint8_t* Data, uint32_t Width, uint32_t Height, uint32_t Channels);