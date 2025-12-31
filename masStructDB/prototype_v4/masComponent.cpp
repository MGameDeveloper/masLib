#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "masComponent.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))
#define MAS_MALLOC(type, size)            (type*)malloc(size)
#define MAS_ENTRY_MAX                     (1024 * 3 * 5)
#define MAS_COMPONENT_MAX                 (1024 * 3)


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
struct masEntry
{
	uint64_t Hash;
	uint32_t Index;
};

struct masComponent
{
	uint32_t  UniqueID;
	uint32_t  Size;
	char      Name[32];
};

struct masComponentMap
{
	masEntry     EntryList[MAS_ENTRY_MAX];
	masComponent ComponentList[MAS_COMPONENT_MAX];
	uint32_t     EntryCount;
	uint32_t     ComponentCount;
	uint32_t     UniqueIDGen;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
static masComponentMap* CompMap = NULL;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint64_t masInternal_Hash(const char* Data, uint64_t Size)
{
	uint64_t FNV1A_64_OFFSET_BASIS = 14695981039346656037ull;
	uint64_t FNV1A_64_PRIME        = 1099511628211ull;

	uint64_t Hash = FNV1A_64_OFFSET_BASIS;
	for (size_t i = 0; i < Size; ++i) 
	{
		Hash ^= Data[i];
		Hash *= FNV1A_64_PRIME;
	}

	return Hash;
}

static const masComponent* masInternal_FindComponent(const char* Name, uint64_t Hash)
{
	uint32_t      EntryIdx  = Hash % MAS_ENTRY_MAX;
	masEntry     *Entry     = &CompMap->EntryList[EntryIdx];
	masComponent *Component = &CompMap->ComponentList[Entry->Index];
	if (Entry->Hash != Hash)
	{
		if(Component->Size != 0)
			printf("[ ERROR ]: Component Name Conflict [ %s with %s ]\n", Name, Component->Name);
		return NULL;
	}

	return Component;
}

const masComponent* masInternal_AddComponent(const char* Name, uint32_t NameLen, uint32_t CompSize, uint32_t NameHash)
{
	if (CompMap->ComponentCount + 1 >= MAS_COMPONENT_MAX)
	{
		printf("[ ERROR ]: No available space in component table for registeration\n");
		return NULL;
	}

	uint32_t      EntryIdx  = NameHash % MAS_ENTRY_MAX;
	masEntry     *Entry     = &CompMap->EntryList[EntryIdx];
	masComponent *Component = &CompMap->ComponentList[Entry->Index];
	if (Entry->Hash != 0)
	{
		printf("[ ERROR ]: Component Name Conflict [ %s with %s ]\n", Name, Component->Name);
		return NULL;
	}

	Entry->Hash  = NameHash;
	Entry->Index = CompMap->ComponentCount++;

	Component       = &CompMap->ComponentList[Entry->Index];
	Component->Size = CompSize;
	Component->UniqueID = CompMap->UniqueIDGen++;
	if (NameLen >= 32)
		NameLen = 32 - 1;
	memcpy(Component->Name, Name, NameLen);

	return Component;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masComponent_Init()
{
	printf("[ INFO ]: Checking Component File\n");
	printf("[ INFO ]: Component File Doesn't Exists\n");
	printf("[ INFO ]: Creating Component File\n");

	//
	masComponentMap *Map = MAS_MALLOC(masComponentMap, sizeof(masComponentMap));
	if (!Map)
	{
		printf("[ ERROR ]: ALLOCATING_COMPONENT_MAP\n");
		return false;
	}
	memset(Map, 0, sizeof(masComponentMap));
	Map->UniqueIDGen = 1;

	CompMap = Map;

	return true;
}

void masComponent_DeInit()
{
	printf("[ TRACE ]: Writing Components to file\n");
	printf("[ ERROR ]: Failed to write components to file\n");
	
	free(CompMap);
	CompMap = NULL;
}

void masComponent_RegisterByName(const char* Name, uint32_t Size)
{
	if (!CompMap)
	{
		printf("[ ERROR ]: You must call masComponent_Init() before any call to masComponent_* functions\n");
		return;
	}

	uint64_t            NameLen   = strlen(Name);
	uint64_t            NameHash  = masInternal_Hash(Name, NameLen);
	const masComponent *FoundComp = masInternal_FindComponent(Name, NameHash);
	if (FoundComp)
	{
		printf("Component[ %s ]: ALREADY_REGISTERED\n", Name);
		return;
	}

	const masComponent* Component = masInternal_AddComponent(Name, NameLen, Size, NameHash);
	if (!Component)
		printf("[ ERROR ]: Registering Component [ %s ]\n", Name);
}

const masComponent* masComponent_Find(const char* Name)
{
	if (!CompMap)
	{
		printf("[ ERROR ]: You must call masComponent_Init() before any call to masComponent_* functions\n");
		return;
	}

	uint64_t NameLen  = strlen(Name);
	uint64_t NameHash = masInternal_Hash(Name, NameLen);
	const masComponent* Component = masComponent_Find(Name);
	if (!Component)
	{
		printf("[ ERROR ]: Finding Component [ %s ]\n", Name);
		return NULL;
	}

	return Component;
}