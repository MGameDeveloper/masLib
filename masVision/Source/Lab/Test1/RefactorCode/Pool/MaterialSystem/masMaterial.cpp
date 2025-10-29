#include "masMaterial.h"
#include "../masPool.h"


/***********************************************************************************
*
************************************************************************************/
static masPool<masMaterial> GMaterials("Materials");


/***********************************************************************************
*
************************************************************************************/
masMaterial::masMaterial(const char* Name) 
{

}

masMaterial::~masMaterial() 
{

}

void masMaterial::SetShader(const masShader& Shader)
{

}

void masMaterial::SetTexture(int32_t TextureID, const masTexture& Texture)
{

}

void masMaterial::UnSetTexture(int32_t TextureID)
{

}

void masMaterial::SetTextureSampler(int32_t TextureID, const masTextureSampler& Sampler)
{

}

const masTexture& masMaterial::GetTexture(int32_t TextureID)
{

}

const masTextureSampler& masMaterial::GetTextureSampler(int32_t TextureID)
{

}

const masMaterialScalars& masMaterial::GetMaterialScalars()
{

}

const masShader& masMaterial::GetShader()
{

}
