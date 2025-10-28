#pragma once

#include "masTextureSampler.h"

class masTexture
{
private:
	masHandle         Handle;
	masTextureSampler Sampler;

public:
	masTexture();
	~masTexture();
	masTexture(const masTexture& Other);
	masTexture(masTexture&& Other);
	masTexture& operator=(const masTexture& Rhs);
	masTexture& operator=(masTexture&& Rhs);

	masTextureSampler GetSampler();
	void SetSampler(masTextureSampler Sampler);
};