#include "masTexture.h"
#include "Graphics/masGraphics.h"
#include "masImageLoader.h"
#include "../masPool.h"


/*********************************************************************************
*
**********************************************************************************/
struct masTextureD3D11
{
	ID3D11Texture2D          *pTexture2D;
	ID3D11ShaderResourceView *pTexture2DView;
	int32_t                   Width;
	int32_t                   Height;
	int32_t                   Channels;
	float                     AspectRatio;
	char                      Name[128];
};


/*********************************************************************************
*
**********************************************************************************/
void masInternal_ReleaseTexture(masTextureD3D11* Texture)
{
	if (Texture)
	{
		Texture->pTexture2D->Release();
		Texture->pTexture2DView->Release();
	}
}

bool masInternal_FindTexture(masTextureD3D11* Texture, const char* Name)
{
	if (Texture && Name)
		return (strcmp(Texture->Name, Name) == 0);
	return false;
}


/*********************************************************************************
*
**********************************************************************************/
static masPool* GTextures = masPool_Create("Textures", sizeof(masTextureD3D11), MAS_POOL_RELEASE_ITEM_FUNC(masInternal_ReleaseTexture));


/*********************************************************************************
*
**********************************************************************************/
MAS_DEFINE_GLOBAL_POOL(GTextures, masTexture, masTextureD3D11);


/*********************************************************************************
*
**********************************************************************************/
MAS_DEFINE_HANDLE(masTexture, GTextures);


/*********************************************************************************
*
**********************************************************************************/
masTexture masTexture_Load(const char* Name)
{
	masTexture Found = GTextures_Find(MAS_POOL_FIND_ITEM_FUNC(masInternal_FindTexture), Name);
	if (GTextures_IsHandleValid(Found))
	{
		GTextures_AddRef(Found);
		return Found;
	}

	// Load the texture it's not in the pool
	masImageLoader Image(4);
	if (Image.LoadByName(Name))
	{
		bool bTextureCreated = false;
		ID3D11Texture2D          *GPUTexture2D     = masGraphics_CreateTexture2D(Image.GetData(), Image.GetWidth(), Image.GetHeight(), Image.GetChannels());
		ID3D11ShaderResourceView *GPUTexture2DView = masGraphics_CreateShaderResourceView_Texture2D(GPUTexture2D);
		if (GPUTexture2D && GPUTexture2DView)
		{
			masTexture Texture = GTextures_Alloc();
			if (masTextureD3D11* Data  = GTextures_Get(Texture))
			{
				Data->pTexture2D     = GPUTexture2D;
				Data->pTexture2DView = GPUTexture2DView;
				Data->Width          = Image.GetWidth();
				Data->Height         = Image.GetHeight();
				Data->Channels       = Image.GetChannels();
				Data->AspectRatio    = (float)Data->Width / (float)Data->Height;

				int32_t NameLen = strlen(Name);
				if (NameLen >= 128)
					NameLen = 128 - 1;
				memcpy(Data->Name, Name, NameLen);
			}
		}
		else
		{
			if (GPUTexture2D)
				GPUTexture2D->Release();
			if (GPUTexture2DView)
				GPUTexture2DView->Release();
		}
	}

	return masTexture();
}

void  masTexture_UnLoad(masTexture* Texture) 
{
	GTextures_Free(Texture);
}

void* masTexture_GetGPUTexture(masTexture Texture)
{
	masTextureD3D11* Data = GTextures_Get(Texture);
	if (!Data)
		return nullptr;
	return Data->pTexture2D;
}

void* masTexture_GetGPUTextureView(masTexture Texture) 
{
	masTextureD3D11* Data = GTextures_Get(Texture);
	if (!Data)
		return nullptr;
	return Data->pTexture2DView;
}

const char* masTexture_GetName(masTexture Texture) 
{
	masTextureD3D11* Data = GTextures_Get(Texture);
	if (!Data)
		return nullptr;
	return Data->Name;
}

int32_t masTexture_GetWidth(masTexture Texture) 
{
	masTextureD3D11* Data = GTextures_Get(Texture);
	if (!Data)
		return 0;
	return Data->Width;
}

int32_t masTexture_GetHeight(masTexture Texture) 
{
	masTextureD3D11* Data = GTextures_Get(Texture);
	if (!Data)
		return 0;
	return Data->Height;
}

int8_t masTexture_GetChannels(masTexture Texture) 
{
	masTextureD3D11* Data = GTextures_Get(Texture);
	if (!Data)
		return 0;
	return Data->Channels;
}

float masTExture_GetAspectRatio(masTexture  Texture)
{
	masTextureD3D11* Data = GTextures_Get(Texture);
	if (!Data)
		return 0.f;
	return Data->AspectRatio;
}