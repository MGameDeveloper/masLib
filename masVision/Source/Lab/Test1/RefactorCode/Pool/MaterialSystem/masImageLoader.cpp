#include "masImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


/***********************************************************************************
*
************************************************************************************/
masImageLoader::masImageLoader(const char* Path, int32_t RequiredChannels) :
	Data(nullptr), Width(0), Height(0), Channels(0)
{
	int32_t w, h, c;
	uint8_t* Image = stbi_load(Path, &w, &h, &c, RequiredChannels);
	if (!Image)
		return;

	Data     = Image;
	Width    = (uint32_t)w;
	Height   = (uint32_t)h;
	Channels = (uint32_t)c;
}

masImageLoader::~masImageLoader()
{
	stbi_image_free(Data);
	Data = nullptr;
	Width    = 0;
	Height   = 0;
	Channels = 0;
}

bool masImageLoader::IsValid()
{
	return (Data != nullptr);
}

uint8_t* masImageLoader::GetData()
{
	return Data;
}

uint32_t masImageLoader::GetWidth()
{
	return Width;
}

uint32_t masImageLoader::GetHeight()
{
	return Height;
}

uint32_t masImageLoader::GetChannels()
{
	return Channels;
}
