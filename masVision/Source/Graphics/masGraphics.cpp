#include "masGraphics.h"
#include "Debug/Assert/masAssert.h"

#include <wrl/client.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

using namespace Microsoft::WRL;

struct masD3D11
{
	ComPtr<ID3D11Device>           Device;
	ComPtr<ID3D11DeviceContext>    ImmediateContext;
	ComPtr<IDXGISwapChain1>        SwapChain;
	ComPtr<ID3D11RenderTargetView> RenderTargetView;
	D3D11_TEXTURE2D_DESC           BackBufferDesc;
	D3D11_VIEWPORT                 Viewport;
};
static masD3D11 D3D11 = { };


/****************************************************************************************************
*
*****************************************************************************************************/
bool masGraphics_Init(const void* Window, uint32_t Width, uint32_t Height)
{
	UINT FACTORY_CREATE_FLAGS = DXGI_CREATE_FACTORY_DEBUG;
	ComPtr<IDXGIFactory2> pFactory2 = nullptr;
	HRESULT hr = CreateDXGIFactory2(FACTORY_CREATE_FLAGS, __uuidof(IDXGIFactory2), &pFactory2);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Create Factory");

	ComPtr<IDXGIFactory7> pFactory7 = nullptr;
	hr = pFactory2.As(&pFactory7);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Query Factory Interface 7");

	ComPtr<IDXGIAdapter4> pAdapter4 = nullptr;
	//hr = pFactory7->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pAdapter4)); // cause problem when running graphics debugging
	
	ComPtr<IDXGIAdapter1> pAdapter1 = nullptr;
	hr = pFactory7->EnumAdapters1(0, pAdapter1.GetAddressOf());
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Enumerate Adaper by GPU Perference");

	hr = pAdapter1.As(&pAdapter4);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Failed to cast IDXGIAdapter1 to IDXGIAdapter4");

	D3D_FEATURE_LEVEL           FeatureLevelList[]  = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1};
	UINT                        FeatureLevelCount   = ARRAYSIZE(FeatureLevelList);
	UINT                        DEVICE_CREATE_FLAGS = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL           FeatureLevel;
	ComPtr<ID3D11Device>        pDevice             = nullptr;
	ComPtr<ID3D11DeviceContext> pImmediateContext            = nullptr;
	hr = D3D11CreateDevice(pAdapter4.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, DEVICE_CREATE_FLAGS, FeatureLevelList, FeatureLevelCount,
		D3D11_SDK_VERSION, &pDevice, &FeatureLevel, &pImmediateContext);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Create Device");
	

	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc1 = { 0 };
	SwapChainDesc1.Width       = Width;
	SwapChainDesc1.Height      = Height;
	SwapChainDesc1.Format      = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc1.Stereo      = FALSE;
	SwapChainDesc1.SampleDesc  = { 1, 0 };
	SwapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc1.BufferCount = 3;
	SwapChainDesc1.Scaling     = DXGI_SCALING_NONE;
	SwapChainDesc1.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	SwapChainDesc1.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
	SwapChainDesc1.Flags       = 0;
	ComPtr<IDXGISwapChain1> pSwapChain1 = nullptr;
	hr = pFactory7->CreateSwapChainForHwnd(pDevice.Get(), (HWND)Window, &SwapChainDesc1, nullptr, nullptr, &pSwapChain1);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Create Swap Chain From Hwnd");

	ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
	hr = pSwapChain1->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Get Back Buffer from Swap Chain");

	ComPtr<ID3D11RenderTargetView> pRenderTargetView = nullptr;
	hr = pDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, &pRenderTargetView);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Create Render Target View");


	D3D11.Device           = pDevice;
	D3D11.ImmediateContext = pImmediateContext;
	D3D11.SwapChain        = pSwapChain1;
	D3D11.RenderTargetView = pRenderTargetView;
	pBackBuffer->GetDesc(&D3D11.BackBufferDesc);
	D3D11.Viewport = { 0.f, 0.f, (float)Width, (float)Height, 0.f, 1.f };


	D3D11.ImmediateContext->OMSetRenderTargets(1, D3D11.RenderTargetView.GetAddressOf(), nullptr);
	D3D11.ImmediateContext->RSSetViewports(1, &D3D11.Viewport);

	return true;
}

void masGraphics_Terminate()
{

}

void masGraphics_Render()
{
	D3D11.SwapChain->Present(0, 0);
}

void masGraphics_Clear(const float Color[4])
{
	ID3D11RenderTargetView* pRTV = D3D11.RenderTargetView.Get();
	D3D11.ImmediateContext->ClearRenderTargetView(pRTV, Color);
}





/************************************************************************************************************
*
*************************************************************************************************************/
enum masShaderType
{
	MAS_SHADER_VERTEX,
	MAS_SHADER_PIXEL
};

struct masShader
{
	ComPtr<ID3DBlob>           ByteCode;
	ComPtr<ID3D11DeviceChild>  Shader;
	const char                *Name;
	const char                *Path;
	const char                *Model;
	masShaderType              Type;

	~masShader()
	{
		if (Shader)
			Shader->Release();
	}
};


static masShader* masShader_Create(const char* Path, const char* Name, const char* EntryPoint, masShaderType ShaderType)
{
	int32_t PathLen = MultiByteToWideChar(CP_UTF8, 0, Path, -1, nullptr, 0);
	wchar_t WPath[512] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, Path, -1, WPath, PathLen);

	// Shader Model Setup
	const char* ShaderModel = nullptr;
	switch (ShaderType)
	{
	case MAS_SHADER_VERTEX: ShaderModel = "vs_5_0"; break;
	case MAS_SHADER_PIXEL:  ShaderModel = "ps_5_0"; break;
	}

	// Compiling Shader Source
	ComPtr<ID3DBlob> ByteCode = nullptr;
	ComPtr<ID3DBlob> Error = nullptr;
	HRESULT hr = D3DCompileFromFile(WPath, nullptr, nullptr, EntryPoint, ShaderModel, 0, 0, &ByteCode, &Error);
	MAS_ASSERT(SUCCEEDED(hr), "Compiling Shader: %s", Error->GetBufferPointer());

	// Create Shader Resorce
	ComPtr<ID3D11DeviceChild> pShader = nullptr;
	switch (ShaderType)
	{
	case MAS_SHADER_VERTEX:
	    {
	    	ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
	    	hr = D3D11.Device->CreateVertexShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), nullptr, &pVertexShader);
			MAS_ASSERT(SUCCEEDED(hr), "Creating VertexShader");
			hr = pVertexShader.As(&pShader);
			MAS_ASSERT(SUCCEEDED(hr), "Casting VertexShader to DeviceChild");
	    }
		break;

	case MAS_SHADER_PIXEL:
	    {
	    	ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
	    	hr = D3D11.Device->CreatePixelShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), nullptr, &pPixelShader);
	    	MAS_ASSERT(SUCCEEDED(hr), "Creating VertexShader");
			hr = pPixelShader.As(&pShader);
			MAS_ASSERT(SUCCEEDED(hr), "Casting PixelShader to DeviceChild");
	    }
		break;
	}

	// 
	masShader* Shader = (masShader*)malloc(sizeof(masShader));
	Shader->Path     = Path;
	Shader->Name     = Name;
	Shader->Model    = ShaderModel;
	Shader->Type     = ShaderType;
	Shader->ByteCode = ByteCode;
	Shader->Shader   = pShader;

	return Shader;
}

masShader* masShader_CreateVertexShader(const char* Path, const char* Name, const char* EntryPoint)
{
	return masShader_Create(Path, Name, EntryPoint, MAS_SHADER_VERTEX);
}

masShader* masShader_CreatePixelShader(const char* Path, const char* Name, const char* EntryPoint)
{
	return masShader_Create(Path, Name, EntryPoint, MAS_SHADER_PIXEL);
}

void masShader_Destroy(masShader** Shader)
{
	if (Shader && *Shader)
	{
		(*Shader)->ByteCode->Release();
		(*Shader)->Shader->Release();
		free(*Shader);
		*Shader = nullptr;
	}
}