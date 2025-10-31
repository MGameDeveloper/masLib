#pragma once

#include <stdint.h>

class masImageLoader
{
private:
	uint8_t* Data;
	int32_t Width;
	int32_t Height;
	int32_t Channels;
	int32_t RequiredChannels;
	char    Path[256];

private:
	void Clean();
	bool FindPath(const char* Name);

public:
	masImageLoader(int32_t RequiredChannels);
	~masImageLoader();
	masImageLoader(const masImageLoader& Other)          = delete;
	masImageLoader(masImageLoader&& Other)               = delete;
	masImageLoader& operator=(const masImageLoader& Rhs) = delete;
	masImageLoader& operator=(masImageLoader&& Rhs)      = delete;

	// Will Loop in the asset directory until it find the file with supplied name
	bool LoadByName(const char* Name);

	// Will Try to load using Path supplied
	bool LoadByPath(const char* Path);

	const uint8_t* GetData();
	int32_t        GetWidth();
	int32_t        GetHeight();
	int32_t        GetChannels();
};