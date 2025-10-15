#pragma once

#include <unordered_map>


/****************************************************************************************************************
*
*****************************************************************************************************************/
struct masResource
{
	std::string Name;
	int32_t     RefCount;

	virtual void Release() = 0;
};


/****************************************************************************************************************
*
*****************************************************************************************************************/
template<typename T>
class masResourceMap
{
private:
	std::unordered_map<std::string, T> ResourceMap;

public:
	masResourceMap() :
		ResourceMap()
	{
	}

	~masResourceMap()
	{ 
		// release every resource in the map properly
	}

	masResourceMap(const masResourceMap& Other)            = delete;
	masResourceMap& operator=(const masResourceMap& Other) = delete;
	masResourceMap(masResourceMap&& Other)                 = delete;
	masResourceMap& operator=(masResourceMap&& Other)      = delete;
	
	T* Find(const std::string& Name)
	{
		auto Found = ResourceMap.find(Name);
		if (Found != ResourceMap.end())
		{
			masResource* Res = (masResource*)&Found->second;
			Res->RefCount++;
			return &Found->second;
		}

		return nullptr;
	}

	T* Create(const std::string& Name)
	{
		auto Found = ResourceMap.find(Name);
		if (Found != ResourceMap.end())
		{
			printf("[ ERROR ]: masResourceMap::Create() Name is already added use masResourceMap::Find() instead\n");
			return nullptr;
		}

		ResourceMap.insert({ Name, T() });
		T* Resource = &ResourceMap[Name];

		masResource* Res = (masResource*)Resource;
		Res->Name        = Name;
		Res->RefCount    = 1;
		return Resource;
	}

	void Destroy(T** Resource)
	{
		if (Resource && *Resource)
		{
			T* pResource = *Resource;
			pResource->Release();

			masResource* Res = (masResource*)pResource;
			Res->RefCount--;
			if (Res->RefCount <= 0)
			{
				printf("[ INFO ]: Resouce( %s ) RefCount reaches ZERO -> REMOVED\n", Res->Name.c_str());
				ResourceMap.erase(Res->Name);
			}

			*Resource = nullptr;		
		}
	}
};
