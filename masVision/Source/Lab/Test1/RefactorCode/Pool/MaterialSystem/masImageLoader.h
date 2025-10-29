#pragma once

#include <stdint.h>

class masImageLoader
{
private:
	uint8_t  *Data;
	uint32_t  Width;
	uint32_t  Height;
	uint32_t  Channels;
	char      Path[256];
	char      Name[128];

public:
	masImageLoader(const char* Path, int32_t RequiredChannels = 0);
	~masImageLoader();
	masImageLoader(const masImageLoader& Other)          = delete;
	masImageLoader(masImageLoader&& Other)               = delete;
	masImageLoader& operator=(const masImageLoader& Rhs) = delete;
	masImageLoader& operator=(masImageLoader&& Rhs)      = delete;

	bool     IsValid();
	uint8_t* GetData();
	uint32_t GetWidth();
	uint32_t GetHeight();
	uint32_t GetChannels();
};