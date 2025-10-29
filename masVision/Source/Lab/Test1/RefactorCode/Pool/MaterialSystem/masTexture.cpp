#include "masTexture.h"
#include "../masPool.h"

#include "Graphics/masGraphics.h"
#include "masImageLoader.h"


/***********************************************************************************
*
************************************************************************************/
struct masTextureData
{
	ID3D11Texture2D          *pTexture2D;
	ID3D11ShaderResourceView *pTexture2DView;
	uint32_t                  Width;
	uint32_t                  Height;
	uint8_t                   Channels;
	char                      Path[256];
};


/***********************************************************************************
*
************************************************************************************/
void masInternal_ReleaseTextureData(masTextureData* Data)
{
	if (Data)
	{
		Data->pTexture2D->Release();
		Data->pTexture2DView->Release();
		Data->pTexture2D     = nullptr;
		Data->pTexture2DView = nullptr;
	}
}

bool masInternal_FindTextureData(masTextureData* Data, const char* Path)
{
	if (!Data || !Path)
		return false;
	return (strcmp(Data->Path, Path) == 0);
}


/***********************************************************************************
*
************************************************************************************/
static masPool<masTextureData> GTextures("Textures", MAS_POOL_ITEM_CLEAN_FUNC(masInternal_ReleaseTextureData));


/***********************************************************************************
*
************************************************************************************/
#define MAS_TEXTURE_GET_DATA(Member, FailReturn)\
    masTextureData* Data = GTextures.GetElement(Handle);\
    if(Data)\
        return Data->Member;\
    return FailReturn;


/***********************************************************************************
*
************************************************************************************/
masTexture::masTexture()
{

}

masTexture::masTexture(const char* Path)
{
	masHandle Found = GTextures.Find(MAS_POOL_ITEM_FIND_FUNC(masInternal_FindTextureData), Path);
	if (Found.IsValid())
	{
		Handle = Found;
		GTextures.AddRef(Handle);
		return;
	}

	masTextureData* Data = GTextures.GetElement(Handle);
	if (Data)
	{
		masImageLoader Image(Path, 4);
		if (Image.IsValid())
		{
			Data->pTexture2D     = masGraphics_CreateTexture2D(Image.GetData(), Image.GetWidth(), Image.GetHeight(), Image.GetChannels());
			Data->pTexture2DView = masGraphics_CreateShaderResourceView_Texture2D(Data->pTexture2D);
			Data->Width          = Image.GetWidth();
			Data->Height         = Image.GetHeight();
			Data->Channels       = Image.GetChannels();
			
			uint32_t PathLen = strlen(Path);
			if (PathLen >= 256)
				PathLen = 256 - 1;
			memcpy(Data->Path, Path, PathLen);
		}
	}
}

masTexture::~masTexture()
{
	GTextures.Free(Handle);
}

masTexture::masTexture(const masTexture& Other)
{
	Handle = Other.Handle;
	GTextures.AddRef(Handle);
}

masTexture::masTexture(masTexture&& Other)
{
	Handle = Other.Handle;
	Other.Handle = {};
}

masTexture& masTexture::operator=(const masTexture& Rhs)
{
	if (this != &Rhs)
	{
		GTextures.Free(Handle);
		Handle = Rhs.Handle;
		GTextures.AddRef(Handle);
	}

	return *this;
}

masTexture& masTexture::operator=(masTexture&& Rhs)
{
	if (this != &Rhs)
	{
		GTextures.Free(Handle);
		Handle = Rhs.Handle;
		Rhs.Handle = {};
	}

	return *this;
}

void*            masTexture::GetTexture()    { MAS_TEXTURE_GET_DATA(pTexture2D,     nullptr); }
void*            masTexture::GetTextureView(){ MAS_TEXTURE_GET_DATA(pTexture2DView, nullptr); }
const char*      masTexture::GetPath()       { MAS_TEXTURE_GET_DATA(Path,           nullptr); }
uint32_t         masTexture::GetWidth()      { MAS_TEXTURE_GET_DATA(Width,          0);       }
uint32_t         masTexture::GetHeight()     { MAS_TEXTURE_GET_DATA(Height,         0);       }
uint8_t          masTexture::GetChannels()   { MAS_TEXTURE_GET_DATA(Channels,       0);       }