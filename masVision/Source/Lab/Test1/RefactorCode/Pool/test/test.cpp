#include "test.h"
#include "../masPool.h"
#include "../../Resources/masTextureDefs.h"

/**************************************************************************************************
*
***************************************************************************************************/
struct masMaterialData
{
	masMaterialScalars Scalars;
	masTexture         Textures[MAS_TEXTURE_COUNT];
	masSampler         pSamplers[MAS_TEXTURE_COUNT];
	masShader          Shader;
};


/**************************************************************************************************
*
***************************************************************************************************/
masPool<masMaterialData, masMaterial> GMaterials("Materials");


/**************************************************************************************************
*
***************************************************************************************************/
masMaterial masMaterial_Create(const char* Name)
{
	return GMaterials.Alloc();
}

void masMaterial_Destroy(masMaterial* Material)
{
	GMaterials.Free(Material);
}

void masMaterial_SetShader(masMaterial Material, masShader Shader)
{
	masMaterialData *Data = GMaterials.GetElement(Material);
	if (Data)
		Data->Shader = Shader;
}

void masMaterial_SetTexture(masMaterial Material, int32_t TextureID, masTexture Texture)
{
	masMaterialData *Data = GMaterials.GetElement(Material);
	if (Data)
		Data->Textures[TextureID] = Texture;
}