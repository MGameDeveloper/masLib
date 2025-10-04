#include "masGraphics.h"
#include "Debug/Assert/masAssert.h"

#include <wrl/client.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>


using namespace Microsoft::WRL;


bool masGraphics_Init(HWND Window, uint32_t Width, uint32_t Height)
{
	UINT FACTORY_CREATE_FLAGS = DXGI_CREATE_FACTORY_DEBUG;
	ComPtr<IDXGIFactory2> pFactory2 = nullptr;
	HRESULT hr = CreateDXGIFactory2(FACTORY_CREATE_FLAGS, __uuidof(IDXGIFactory2), &pFactory2);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Create Factory");

	ComPtr<IDXGIFactory7> pFactory7 = nullptr;
	hr = pFactory2.As(&pFactory7);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Query Factory Interface 7");

	ComPtr<IDXGIAdapter4> pAdapter4 = nullptr;
	hr = pFactory7->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pAdapter4));
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Enumerate Adaper by GPU Perference");

	D3D_FEATURE_LEVEL           FeatureLevelList[]  = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1};
	UINT                        FeatureLevelCount   = ARRAYSIZE(FeatureLevelList);
	UINT                        DEVICE_CREATE_FLAGS = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL           FeatureLevel;
	ComPtr<ID3D11Device>        pDevice             = nullptr;
	ComPtr<ID3D11DeviceContext> pContext            = nullptr;
	hr = D3D11CreateDevice(pAdapter4.Get(), D3D_DRIVER_TYPE_HARDWARE, NULL, DEVICE_CREATE_FLAGS, FeatureLevelList, FeatureLevelCount,
		D3D11_SDK_VERSION, &pDevice, &FeatureLevel, &pContext);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Create Device");

	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc1 = { 0 };
	SwapChainDesc1.Width       = Width;
	SwapChainDesc1.Height      = Height;
	SwapChainDesc1.Format      = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc1.Stereo      = FALSE;
	SwapChainDesc1.SampleDesc  = { 0, 1 };
	SwapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc1.BufferCount = 3;
	SwapChainDesc1.Scaling     = DXGI_SCALING_NONE;
	SwapChainDesc1.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	SwapChainDesc1.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
	SwapChainDesc1.Flags       = 0;
	ComPtr<IDXGISwapChain1> pSwapChain1 = nullptr;
	hr = pFactory7->CreateSwapChainForHwnd(pDevice.Get(), Window, &SwapChainDesc1, nullptr, nullptr, &pSwapChain1);
	MAS_ASSERT(SUCCEEDED(hr), "DX11 Create Swap Chain From Hwnd");



	return true;
}

void masGraphics_Terminate()
{

}