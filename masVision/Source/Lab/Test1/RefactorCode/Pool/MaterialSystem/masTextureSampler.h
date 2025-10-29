#pragma once

#include "../masHandle.h"


class masTextureSampler
{
private:
	masHandle Handle;

public:
	masTextureSampler();
	~masTextureSampler();
	masTextureSampler(const masTextureSampler& Other);
	masTextureSampler(masTextureSampler&& Other);
	masTextureSampler& operator=(const masTextureSampler& Rhs);
	masTextureSampler& operator=(masTextureSampler&& Rhs);

	void* GetSampler();
};