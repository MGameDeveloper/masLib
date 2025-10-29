#pragma once

#include "masTextureSampler.h"

class masTexture
{
private:
	masHandle Handle;

public:
	masTexture();
	masTexture(const char* Path);
	~masTexture();
	masTexture(const masTexture& Other);
	masTexture(masTexture&& Other);
	masTexture& operator=(const masTexture& Rhs);
	masTexture& operator=(masTexture&& Rhs);

	void*            GetTexture();
	void*            GetTextureView();
	const char*      GetPath();
	uint32_t         GetWidth();
	uint32_t         GetHeight();
	uint8_t          GetChannels();
};