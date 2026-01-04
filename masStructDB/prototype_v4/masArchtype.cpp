#include <string.h>
#include <stdio.h>

#include "masArchtype.h"
#include "masComponent.h"
#include "masCoreComponents.h"
#include "internal/masChunk.h"
#include "internal/masMemory.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))
#define MAS_ENTRY_MAX    (1024 * 10)


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
	masComponentList ComponentList;
	masChunkListID   ChunkList;
	uint32_t         MaxEntityCount;
	uint32_t         MemSize;
};

struct masEntry
{
	uint64_t Hash;
	int32_t  Index;
};

struct masArchtypeMap
{
	uint8_t  *ArchtypeBuf;
	masEntry  EntryList[MAS_ENTRY_MAX];
	uint32_t  EntryCount;
	uint32_t  ArchtypeBufIdx;
	uint32_t  MaxArchtypeBufSize;
	uint32_t  ArchtypeCount;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
static masArchtypeMap *GMap = NULL;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool masInternal_ResizeArchtypeBuf()
{
	if (!GMap)
		return false;

	size_t MemSize = GMap->MaxArchtypeBufSize * 2;
	if (MAS_REALLOC(uint8_t, GMap->ArchtypeBuf, MemSize))
	{
		GMap->MaxArchtypeBufSize = MemSize;
		return true;
	}

	return false;
}

static masArchtype* masInternal_FindArchtype(const masComponentList* ComponentList)
{
	if (!GMap)
		return NULL;

	uint32_t     EntryIdx = ComponentList->Hash % MAS_ENTRY_MAX;
	masEntry    *Entry    = &GMap->EntryList[EntryIdx];
	masArchtype *Archtype = MAS_PTR_OFFSET(masArchtype, GMap->ArchtypeBuf, Entry->Index);
	if (Archtype->ComponentList.Hash != ComponentList->Hash)
	{
		printf("[ WARNING ]: NO_ARCHTYPE_FOUND_FOR\n\t [ %s ]\n", ComponentList->Names);
		return NULL;
	}

	return Archtype;
}

static masArchtype* masInternal_CreateArchtype(const masComponentList* ComponentList)
{
	if (!GMap)
		return NULL;

	masArchtype* Archtype = masInternal_FindArchtype(ComponentList);
	if (Archtype)
		return Archtype;

	size_t MemSize = sizeof(masArchtype) + ComponentList->NameLen + (sizeof(masComponent) * ComponentList->Count);
	if (GMap->ArchtypeBufIdx + MemSize >= GMap->MaxArchtypeBufSize)
	{
		if (!masInternal_ResizeArchtypeBuf())
			return NULL;
	}
	MemSize++; // inserting \0 between archtype allocations

	Archtype                 = MAS_PTR_OFFSET(masArchtype, GMap->ArchtypeBuf, GMap->ArchtypeBufIdx);
	Archtype->ChunkList      = masChunkList_Create();
	if (!masChunkList_IsValid(Archtype->ChunkList))
		return NULL;
	Archtype->MaxEntityCount = masChunkList_ChunkSize() / ComponentList->ComponentsSize;
	Archtype->MemSize        = MemSize;

	masComponentList* CompList = &Archtype->ComponentList;
	CompList->Components     = MAS_PTR_OFFSET(masComponent, Archtype,             sizeof(masArchtype));
	CompList->Names          = MAS_PTR_OFFSET(char,         CompList->Components, sizeof(masComponent) * ComponentList->Count);
	CompList->NameLen        = ComponentList->NameLen;
	CompList->Hash           = ComponentList->Hash;
	CompList->Count          = ComponentList->Count;
	CompList->ComponentsSize = ComponentList->ComponentsSize;

	memcpy(CompList->Components, ComponentList->Components, sizeof(masComponent) * ComponentList->Count);
	memcpy(CompList->Names,      ComponentList->Names,      ComponentList->NameLen);


	GMap->ArchtypeBufIdx += Archtype->MemSize;

	return Archtype;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masArchtype_Init()
{
	if (GMap)
		return true;

	size_t  MemSize = sizeof(masArchtypeMap);
	void   *Data    = MAS_MALLOC(void, MemSize);
	if (!Data)
		return false;
	memset(Data, 0, MemSize);

	GMap = (masArchtypeMap*)Data;

	for (uint32_t i = 0; i < MAS_ENTRY_MAX; ++i)
		GMap->EntryList[i].Index = -1;

	size_t MaxArchtypeBufSize = sizeof(masArchtype) * 8129;
	GMap->ArchtypeBuf         = MAS_MALLOC(uint8_t, MaxArchtypeBufSize);
	if (!GMap->ArchtypeBuf)
	{
		MAS_FREE(GMap);
		return false;
	}
	memset(GMap->ArchtypeBuf, 0, MaxArchtypeBufSize);
	GMap->MaxArchtypeBufSize  = MaxArchtypeBufSize;
	GMap->ArchtypeBufIdx      = 0;
	
	if (!masChunkList_Init())
	{
		MAS_FREE(GMap);
		return false;
	}

	return true;
}

void masArchtype_DeInit()
{
	if (!GMap)
		return;

	masChunkList_DeInit();
	//masArchtype* Archtype = MAS_PTR_OFFSET(masArchtype, GMap->ArchtypeBuf, 0);
	//for (uint32_t i = 0; i < GMap->ArchtypeCount; ++i)
	//{
	//	masChunkList_Free(Archtype->ChunkList);
	//	Archtype = MAS_PTR_OFFSET(masArchtype, Archtype, Archtype->MemSize);
	//}

	MAS_FREE(GMap->ArchtypeBuf);
	MAS_FREE(GMap);
	GMap = NULL;
}

uint64_t masArchtype_CreateEntity(const char* Components)
{
	MAS_DECLARE_COMPONENTS(DefaultComponents, masPosition, masRotation, masScale, masWorldMatrix, masLocalMatrix, masParent);

	char FinalComponents[256] = { };
	strcpy(FinalComponents, DefaultComponents);
	if (Components != NULL)
		strcpy(FinalComponents + strlen(DefaultComponents), Components);

	const masComponentList *ComponentList = masComponent_Query(FinalComponents);
	
	masArchtype* Archtype = masInternal_FindArchtype(ComponentList);
	if (!Archtype)
	{
		Archtype = masInternal_CreateArchtype(ComponentList);
		if (!Archtype)
			return 0;
	}

	// get empry entity index from it

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