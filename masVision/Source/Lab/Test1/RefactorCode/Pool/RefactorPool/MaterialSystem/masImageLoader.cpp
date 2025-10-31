#include "masImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <filesystem>


/***********************************************************************************
*
************************************************************************************/
void masImageLoader::Clean()
{
	if (Data)
	{
		stbi_image_free(Data);
		Data = nullptr;
		Width = 0;
		Height = 0;
		Channels = 0;
		memset(Path, 0, 256);
	}
}

bool masImageLoader::FindPath(const char* Name)
{
	memset(Path, 0, 256);

	std::string CurrentPath = std::filesystem::current_path().string();
	for (const auto& Entry : std::filesystem::recursive_directory_iterator(CurrentPath))
	{
		if (Entry.is_regular_file())
		{
			std::string FileName = Entry.path().filename().string();
			if ( FileName == Name)
			{
				memcpy(Path, FileName.data(), FileName.size());
				return true;
			}
		}
	}

	return false;
}

masImageLoader::masImageLoader(int32_t RequiredChannels) :
	Data(nullptr), Width(0), Height(0), Channels(0), RequiredChannels(RequiredChannels), Path({})
{
}

masImageLoader::~masImageLoader()
{

}

bool masImageLoader::LoadByName(const char* Name)
{
	Clean();

	if (!FindPath(Name))
		return false;

	int32_t w, h, c;
	Data = stbi_load(Path, &Width, &Height, &Channels, RequiredChannels);
	if (!Data)
		return false;

	return true;
}

bool masImageLoader::LoadByPath(const char* Path)
{
	Clean();

	int32_t w, h, c;
	Data = stbi_load(Path, &Width, &Height, &Channels, RequiredChannels);
	if (!Data)
		return false;

	return true;
}

masImageLoader::~masImageLoader()       { Clean();        }
const uint8_t* masImageLoader::GetData(){ return Data;    }
int32_t masImageLoader::GetWidth()      { return Width;   }
int32_t masImageLoader::GetHeight()     { return Height;  }
int32_t masImageLoader::GetChannels()   { return Channels;}
