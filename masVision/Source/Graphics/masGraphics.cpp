#include "masGraphics.h"
#include "Debug/Assert/masAssert.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

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

	/*
	* IMGUI
	*/
	D3D11.Device           = pDevice;
	D3D11.ImmediateContext = pImmediateContext;
	D3D11.SwapChain        = pSwapChain1;
	D3D11.RenderTargetView = pRenderTargetView;

	ImGui_ImplWin32_EnableDpiAwareness();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init((void*)Window);
	ImGui_ImplDX11_Init(pDevice.Get(), pImmediateContext.Get());

	return true;
}

void masGraphics_Terminate()
{

}

const masD3D11* masGraphics_D3D11()
{
	return &D3D11;
}

void masGraphicsUI_Prepare()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void masGraphicsUI_Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void masGraphics_Present()
{
	D3D11.SwapChain->Present(0, 0);
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

ComPtr<ID3D11SamplerState> masGraphics_CreateSamplerState()
{
	const masD3D11* x = masGraphics_D3D11();

	D3D11_SAMPLER_DESC SamplerDesc = { };
	SamplerDesc.Filter         = D3D11_FILTER_ANISOTROPIC;
	SamplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MipLODBias     = 0.0f;
	SamplerDesc.MaxAnisotropy  = 8; // or 16 for high-end
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamplerDesc.BorderColor[0] = 0.0f;
	SamplerDesc.BorderColor[1] = 0.0f;
	SamplerDesc.BorderColor[2] = 0.0f;
	SamplerDesc.BorderColor[3] = 0.0f;
	SamplerDesc.MinLOD         = 0;
	SamplerDesc.MaxLOD         = D3D11_FLOAT32_MAX;


	ComPtr<ID3D11SamplerState> pSampler = nullptr;
	HRESULT hr = x->Device->CreateSamplerState(&SamplerDesc, &pSampler);
	MAS_ASSERT(SUCCEEDED(hr), "CREATE_SAMPLER_STATE");

	return pSampler;
}

VOID F()
{
	D3D11_SAMPLER_DESC WrapAniso = {};
	WrapAniso.Filter = D3D11_FILTER_ANISOTROPIC;
	WrapAniso.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	WrapAniso.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	WrapAniso.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	WrapAniso.MaxAnisotropy = 8; // or 16 for high-end
	WrapAniso.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	WrapAniso.MinLOD = 0;
	WrapAniso.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11_SAMPLER_DESC ClampLinear = {};
	ClampLinear.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ClampLinear.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ClampLinear.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ClampLinear.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ClampLinear.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	ClampLinear.MinLOD = 0;
	ClampLinear.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11_SAMPLER_DESC ShadowCompare = {};
	ShadowCompare.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	ShadowCompare.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	ShadowCompare.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	ShadowCompare.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	ShadowCompare.BorderColor[0] = 1.0f;
	ShadowCompare.BorderColor[1] = 1.0f;
	ShadowCompare.BorderColor[2] = 1.0f;
	ShadowCompare.BorderColor[3] = 1.0f;
	ShadowCompare.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	ShadowCompare.MinLOD = 0;
	ShadowCompare.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11_SAMPLER_DESC PointClamp = {};
	PointClamp.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	PointClamp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	PointClamp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	PointClamp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	PointClamp.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	PointClamp.MinLOD = 0;
	PointClamp.MaxLOD = D3D11_FLOAT32_MAX;
}

#if 0
struct masMaterial
{
	ID3D11SamplerState**       Samplers; // MAS_SAMPLER_COUNT
	ID3D11ShaderResourceView** Textures; // MAS_TEXTURE_COUNT
	ID3D11VertexShader*        VertexShader;
	ID3D11PixelShader*         PixelShader;
};

#define MAS_TEXTURE_COUNT 11
#define MAS_SAMPLER_COUNT 4

struct masMaterial
{
	int32_t Textures[MAS_TEXTURE_COUNT];
	int32_t Samplers[MAS_TEXTURE_COUNT];
	int32_t Shader;
	float   BaseColor[4];
	float   Roughness;
	float   Metallic;
	float   EmissiveIntensity;
	float   Opacity;
	float   Clearcoat;
	float   ClearcoatRoughness;
	float   Anisotropy;
	float   Sheen;
	// uint32_t PermutationHash;
};

template<typename T>
struct masHandle
{
	int32_t  Idx;  // index into the pool
	uint32_t Type; // direct you to the correct pool

	const T& Data() { return T(); }
};

masHandle<masMaterial> masModel_Load("RotaryCannon");
#endif