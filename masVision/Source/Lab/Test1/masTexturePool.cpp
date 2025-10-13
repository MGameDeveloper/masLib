#include "Graphics/masGraphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <unordered_map>


/****************************************************************************************
*
*****************************************************************************************/
struct masTexture
{
	const char                       *Path;
	ComPtr<ID3D11Texture2D>           pTexture2D;
	ComPtr<ID3D11ShaderResourceView>  pSRV;
	int32_t                           RefCount;
};


/****************************************************************************************
*
*****************************************************************************************/
static std::unordered_map<const char*, masTexture> TextureMap;


/****************************************************************************************
*
*****************************************************************************************/
masTexture* masTexture_Load(const char* Path)
{
	auto Found = TextureMap.find(Path);
	if (Found != TextureMap.end())
	{
		Found->second.RefCount++;
		return &Found->second;
	}

	const masD3D11* dx = masGraphics_D3D11();

	int32_t x, y, c;
	uint8_t* Image = stbi_load(Path, &x, &y, &c, 0);
	MAS_ASSERT(Image, "stbi_load: %s", Path);
	MAS_ASSERT(c == 4, "RGBA texture only supported");

	//
	D3D11_TEXTURE2D_DESC ImageDesc = { };
	ImageDesc.Width          = x;
	ImageDesc.Height         = y;
	ImageDesc.MipLevels      = 1;
	ImageDesc.ArraySize      = 1;
	ImageDesc.Format         = DXGI_FORMAT_R8G8B8A8_UNORM;
	ImageDesc.SampleDesc     = { 1, 0 };
	ImageDesc.Usage          = D3D11_USAGE_DEFAULT;
	ImageDesc.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
	ImageDesc.CPUAccessFlags = 0;
	ImageDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA ImageData = { };
	ImageData.pSysMem     = Image;
	ImageData.SysMemPitch = x * c;
	ImageData.SysMemSlicePitch;

	ComPtr<ID3D11Texture2D> pTexture2D = nullptr;
	HRESULT hr = dx->Device->CreateTexture2D(&ImageDesc, &ImageData, &pTexture2D);
	MAS_ASSERT(SUCCEEDED(hr), "Creating Texture2D: %s", Path);

	//
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = { };
	SRVDesc.Format              = ImageDesc.Format;
	SRVDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	ComPtr<ID3D11ShaderResourceView> pSRV = nullptr;
	hr = dx->Device->CreateShaderResourceView(pTexture2D.Get(), nullptr, &pSRV);
	MAS_ASSERT(SUCCEEDED(hr), "Create ResourceView for: %s", Path);

	masTexture Texture;
	Texture.Path       = Path;
	Texture.RefCount   = 1;
	Texture.pTexture2D = pTexture2D;
	Texture.pSRV       = pSRV;

	TextureMap.insert({ Path, Texture });

	return &TextureMap[Path];
}

void masTexture_UnLoad(masTexture** Texture)
{
	if (Texture && *Texture)
	{
		masTexture* Tex = *Texture;
		auto Found = TextureMap.find(Tex->Path);
		if (Found != TextureMap.end())
		{
			Found->second.RefCount--;
			if (Found->second.RefCount <= 0)
			{
				Found->second.pTexture2D->Release();
				TextureMap.erase(Tex->Path);
			}
		}

		*Texture = nullptr;
	}
}