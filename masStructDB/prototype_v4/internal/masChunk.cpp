#include <string.h>
#include "masChunk.h"
#include "masMemory.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))
#define MAS_CHUNK_SIZE (1024 * 16)


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
union masChunkListHandle
{
	uint64_t Handle;
	struct
	{
		uint32_t ListIdx;
		uint32_t Version;
	};
};

struct masChunkList
{
	void     **Chunks;
	uint32_t   MaxCount;
	uint32_t   Count;
	uint32_t   Version;
};

struct masChunkMap
{
	masChunkList   *Lists;
	uint32_t       *FreeIndices;
	uint32_t        FreeIndexCount;
	uint32_t        ListCount;
	uint32_t        MaxListCount;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
static masChunkMap* GMap = NULL;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masInternal_Resize(uint32_t Count, uint32_t CountMultiplier)
{
	size_t       MaxListCount = Count * CountMultiplier;
	size_t       MemSize      = sizeof(masChunkMap) + (MaxListCount * sizeof(masChunkList)) + (MaxListCount * sizeof(uint32_t));
	masChunkMap *Map          = MAS_MALLOC(masChunkMap, MemSize);
	if (!Map)
		return false;
	memset(Map, 0, MemSize);

	Map->FreeIndices    = MAS_PTR_OFFSET(uint32_t,     Map,              sizeof(masChunkMap));
	Map->Lists          = MAS_PTR_OFFSET(masChunkList, Map->FreeIndices, sizeof(uint32_t) * MaxListCount);
	Map->ListCount      = 0;
	Map->FreeIndexCount = 0;
	Map->MaxListCount   = MaxListCount;
	
	if (!GMap)
		GMap = Map;
	else
	{
		memcpy(Map->FreeIndices, GMap->FreeIndices, sizeof(uint32_t)     * GMap->MaxListCount);
		memcpy(Map->Lists,       GMap->Lists,       sizeof(masChunkList) * GMap->MaxListCount);
		Map->FreeIndexCount = GMap->FreeIndexCount;
		Map->ListCount      = GMap->ListCount;

		MAS_FREE(GMap);
		GMap = Map;
	}

	return true;
}

bool masInternal_InitChunkList(masChunkList* List)
{
	if (List->Version == 0)
		List->Version = 1;

	if (List->Chunks)
		return true;

	// Allocate Chunks' Pointer List
	List->MaxCount = 2;
	List->Chunks   = MAS_MALLOC(void*, sizeof(void*) * List->MaxCount);
	if (!List->Chunks)
	{
		List->MaxCount = 0;
		return false;
	}

	// Allocate Every Chunk Page Memory
	for (uint32_t ChunkIdx = 0; ChunkIdx < List->MaxCount; ++ChunkIdx)
	{
		void* Chunk = MAS_MALLOC(void, MAS_CHUNK_SIZE);
		if (Chunk)
		{
			memset(Chunk, 0, MAS_CHUNK_SIZE);
			List->Chunks[ChunkIdx] = Chunk;
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masChunkList_Init()
{
	if (!GMap)
	{
		if (!masInternal_Resize(5000, 1))
			return false;
	}

	return true;
}

void masChunkList_DeInit()
{
	if (!GMap)
		return;

	for (uint32_t ListIdx = 0; ListIdx < GMap->MaxListCount; ++ListIdx)
	{
		masChunkList* List = &GMap->Lists[ListIdx];
		if (!List->Chunks)
			continue;

		for (uint32_t ChunkIdx = 0; ChunkIdx < List->MaxCount; ++ChunkIdx)
		{
			MAS_FREE(List->Chunks[ChunkIdx]);
			List->Chunks[ChunkIdx] = NULL;
		}

		MAS_FREE(List->Chunks);
		List->Chunks = NULL;
	}

	MAS_FREE(GMap);
	GMap = NULL;
}

masChunkListID masChunkList_Create()
{
	if (!GMap)
		return { 0 };

	uint32_t ListIdx = 0;
	if (GMap->ListCount >= GMap->MaxListCount)
	{
		if (GMap->FreeIndexCount > 0)
			ListIdx = GMap->FreeIndices[--GMap->FreeIndexCount];
		else
		{
			if (!masInternal_Resize(GMap->MaxListCount, 2))
				return { 0 };
			ListIdx = GMap->ListCount++;
		}
	}
	else
		ListIdx = GMap->ListCount++;

	masChunkList* List = &GMap->Lists[ListIdx];
	if (!masInternal_InitChunkList(List))
	{
		GMap->FreeIndices[GMap->FreeIndexCount++] = ListIdx;
		return { 0 };
	}

	masChunkListHandle ListHandle = { };
	ListHandle.ListIdx = ListIdx;
	ListHandle.Version = List->Version;

	return { ListHandle.Handle };
}

void masChunkList_Free(masChunkListID ChunkListID)
{
	if (!GMap)
		return;

	masChunkListHandle ListHandle = { ChunkListID.ID };
	if (ListHandle.ListIdx >= GMap->MaxListCount || ListHandle.Handle == 0 || ListHandle.Version == 0)
		return;

	masChunkList* List = &GMap->Lists[ListHandle.ListIdx];
	if (List->Version != ListHandle.Version)
		return;

	List->Count = 0;
	List->Version++;
	if (List->Version == 0)
		List->Version = 1;

	GMap->FreeIndices[GMap->FreeIndexCount++] = ListHandle.ListIdx;
}

size_t masChunkList_ChunkSize()
{
	return MAS_CHUNK_SIZE;
}

bool masChunkList_IsValid(masChunkListID ChunkListID)
{
	masChunkListHandle ChunkHandle = { ChunkListID.ID };
	return (ChunkHandle.Version != 0);
}