#include "masGraphics.h"
#include "Debug/Assert/masAssert.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <wrl/client.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

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

	return true;
}

void masGraphics_Terminate()
{

}

const masD3D11* masGraphics_D3D11()
{
	return &D3D11;
}

ComPtr<ID3D11Buffer> masGraphics_CreateVertexBuffer(const void* Vertices, uint32_t VertexCount, uint64_t VertexSize)
{
	const masD3D11* x = &D3D11;

	D3D11_BUFFER_DESC Desc = { };
	Desc.ByteWidth           = VertexCount * VertexSize;
	Desc.Usage               = D3D11_USAGE_DEFAULT;
	Desc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags      = 0; 
	Desc.MiscFlags           = 0;
	Desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA Data = { };
	Data.pSysMem          = Vertices;
	Data.SysMemPitch      = 0;
	Data.SysMemSlicePitch = 0;

	ComPtr<ID3D11Buffer> pVertices = nullptr;
	HRESULT hr = x->Device->CreateBuffer(&Desc, &Data, &pVertices);
	MAS_ASSERT(SUCCEEDED(hr), "GRAPHICS_CREATE_MESH_VERTICES: FAILED");

	return pVertices;
}

ComPtr<ID3D11Buffer> masGraphics_CreateIndexBuffer(const uint32_t* Indices, uint32_t IndexCount)
{
	const masD3D11* x = &D3D11;

	D3D11_BUFFER_DESC Desc = { };
	Desc.ByteWidth           = IndexCount * sizeof(uint32_t);
	Desc.Usage               = D3D11_USAGE_DEFAULT;
	Desc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	Desc.CPUAccessFlags      = 0;
	Desc.MiscFlags           = 0;
	Desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA Data = { };
	Data.pSysMem          = Indices;
	Data.SysMemPitch      = 0;
	Data.SysMemSlicePitch = 0;

	ComPtr<ID3D11Buffer> pIndices = nullptr;
	HRESULT hr = x->Device->CreateBuffer(&Desc, &Data, &pIndices);
	MAS_ASSERT(SUCCEEDED(hr), "GRAPHICS_CREATE_MESH_INDICES: FAILED");

	return pIndices;
}

ComPtr<ID3D11Texture2D> masGraphics_CreateTexture2D(const char* Path)
{
	int32_t RequiredChannels = 4;
	int32_t w, h, c;
	uint8_t* Image = stbi_load(Path, &w, &h, &c, RequiredChannels);
	MAS_ASSERT(Image, "STBI_LOAD: Image -> %s", Path);

	D3D11_TEXTURE2D_DESC ImageDesc = { };
	ImageDesc.Width          = w;
	ImageDesc.Height         = h;
	ImageDesc.MipLevels      = 1;
	ImageDesc.ArraySize      = 1;
	ImageDesc.Format         = DXGI_FORMAT_R8G8B8A8_UNORM;
	ImageDesc.SampleDesc     = { 1, 0 };
	ImageDesc.Usage          = D3D11_USAGE_DEFAULT;
	ImageDesc.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
	ImageDesc.CPUAccessFlags = 0;
	ImageDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA ImageData = { };
	ImageData.pSysMem          = Image;
	ImageData.SysMemPitch      = w * RequiredChannels;
	ImageData.SysMemSlicePitch = 0;

	ComPtr<ID3D11Texture2D> pTexture2D = nullptr;
	HRESULT hr = D3D11.Device->CreateTexture2D(&ImageDesc, &ImageData, &pTexture2D);
	MAS_ASSERT(SUCCEEDED(hr), "Creating Texture2D: %s", Path);

	stbi_image_free(Image);
	Image = nullptr;

	return pTexture2D;
}

ComPtr<ID3D11ShaderResourceView> masGraphics_CreateShaderResourceView_Texture2D(ComPtr<ID3D11Texture2D> pTexture2D)
{
	ComPtr<ID3D11ShaderResourceView> pSRV = nullptr;
	HRESULT hr = D3D11.Device->CreateShaderResourceView(pTexture2D.Get(), nullptr, &pSRV);
	MAS_ASSERT(SUCCEEDED(hr), "Create ResourceView");

	return pSRV;
}