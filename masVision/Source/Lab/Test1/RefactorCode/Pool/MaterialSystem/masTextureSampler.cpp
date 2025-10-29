#include "masTextureSampler.h"
#include "Graphics/masGraphics.h"
#include "../masPool.h"


/***********************************************************************************
*
************************************************************************************/
struct masSamplerData
{
	ID3D11SamplerState* pSampler;
};


/***********************************************************************************
*
************************************************************************************/
void masInternal_ReleaseSamplerData(masSamplerData* Data)
{
	if (Data)
	{
		Data->pSampler->Release();
		Data->pSampler = nullptr;
	}
}


/***********************************************************************************
*
************************************************************************************/
static masPool<masSamplerData> GSamplers("TextureSamplers", MAS_POOL_ITEM_CLEAN_FUNC(masInternal_ReleaseSamplerData));


/***********************************************************************************
*
************************************************************************************/
masTextureSampler::masTextureSampler()
{

}

masTextureSampler::~masTextureSampler()
{

}

masTextureSampler::masTextureSampler(const masTextureSampler& Other)
{

}

masTextureSampler::masTextureSampler(masTextureSampler&& Other)
{

}

masTextureSampler& masTextureSampler::operator=(const masTextureSampler& Rhs)
{

}

masTextureSampler& masTextureSampler::operator=(masTextureSampler&& Rhs)
{

}

void* masTextureSampler::GetSampler()
{
	return nullptr;
}
