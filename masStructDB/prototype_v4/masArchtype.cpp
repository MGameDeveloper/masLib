#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "masArchtype.h"
#include "masComponent.h"
#include "masCoreComponents.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
union masEntityHandle
{
	uint64_t Entity;
	struct
	{
		uint32_t MapperIdx;
		uint32_t Version;
	};
};

struct masEntityMapper
{
	uint32_t Archtype;
	uint32_t ChunkIdx;
	uint32_t EntityIdx;
	uint32_t Version;
};

struct masChunk
{
	void     *Components;
	uint32_t  EntityCount;
};

struct masArchtype
{
	masChunk **Chunks;
	uint32_t   ChunkCount;
	uint32_t   MaxEntityCount;

	char Components[256]; // list of components seperated by comma need to be parsed to get every component size
};

struct masArchtypeList
{
	masArchtype **Archtypes;
	uint32_t      ArchtypeCount;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masArchtype_Init()
{
	return true;
}

void masArchtype_DeInit()
{

}

uint64_t masArchtype_CreateEntity(const char* Components)
{
	MAS_DECLARE_COMPONENTS(DefaultComponents, masPosition, masRotation, masScale, masWorldMatrix, masLocalMatrix, masParent);

	char FinalComponents[256] = { };
	strcpy(FinalComponents, DefaultComponents);
	if (Components != NULL)
		strcpy(FinalComponents + strlen(DefaultComponents), Components);

	const masComponentList *ComponentList = masComponent_Query(FinalComponents);
	
	return 0;
}

void masArchtype_DestroyEntity(uint64_t Entity) 
{
}

void* masArchtype_AddEntityComponent(uint64_t Entity, const char* Component)
{
	return NULL;
}

void masArchtype_RemoveEntityComponent(uint64_t Entity, const char* Component)
{

}

void* masArchtype_GetEntityComponent(uint64_t Entity, const char* Component)
{
	return NULL;
}

void masArchtype_AddEntityChild(uint64_t Entity, uint64_t Child)
{

}

void masArchtype_RemoveEntityChild(uint64_t Child)
{

}