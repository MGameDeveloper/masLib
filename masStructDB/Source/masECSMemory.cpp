#include "masECSMemory.h"
#include <stdlib.h>
#include <string.h>


////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_MALLOC(type, size)            (type*)malloc(size)
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_KB(n)                         (1024llu * n )
#define MAS_FRAME_MEMORY_SIZE              MAS_KB(16)
#define MAS_PAGE_MAX_COUNT                 128


////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
typedef union masMemoryID
{
	uint32_t ID;
	struct
	{
		uint16_t Index;
		uint16_t Generation;
	};
};

typedef struct masPage
{
	void     *Data;
	uint32_t  Size;
	uint32_t  Generation;
};

typedef struct masMemory
{
	uint8_t Frame[MAS_FRAME_MEMORY_SIZE];
	masPage PageList[MAS_PAGE_MAX_COUNT];
	int32_t FreeIDList[MAS_PAGE_MAX_COUNT];
	int32_t PageIdx;
	int32_t FreeIDCount;
	int32_t FrameAllocIdx;
};


////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
static masMemory *GMemory = { };


////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool masECSMemory_Init()
{
	if (GMemory)
		return true;

	size_t MemSize = sizeof(masMemory);
	GMemory = MAS_MALLOC(masMemory, MemSize);
	if (!GMemory)
		return false;
	memset(GMemory, 0, MemSize);

	return true;
}

void masECSMemory_DeInit()
{
	if (GMemory)
	{
		free(GMemory);
		GMemory = NULL;
	}
}

masECSMemoryID masECSMemory_AllocPage()
{
	if (!GMemory)
		return { 0 };

	int32_t Index = -1;
	if (GMemory->FreeIDCount > 0)
	{
		Index = GMemory->FreeIDList[--GMemory->FreeIDCount];
		GMemory->FreeIDList[GMemory->FreeIDCount + 1] = -1;
	}
	else
	{
		if (GMemory->PageIdx < MAS_PAGE_MAX_COUNT)
			Index = GMemory->PageIdx++;
	}

	if (Index == -1)
		return { 0 };

	masPage* Page = &GMemory->PageList[Index];
	if (!Page->Data)
	{
		Page->Data = MAS_MALLOC(void, MAS_KB(16));
		if (!Page->Data)
		{
			GMemory->FreeIDList[GMemory->FreeIDCount++] = Index;
			return { 0 };
		}

		Page->Generation = 1;
		Page->Size       = MAS_KB(16);
	}

	masMemoryID MemoryID = { };
	MemoryID.Index      = Index;
	MemoryID.Generation = Page->Generation;

	return MemoryID.ID;
}

void masECSMemory_FreePage(masECSMemoryID ID)
{
	if (!GMemory)
		return;

	masMemoryID MemoryID = { ID };
	if (MemoryID.ID == 0)
		return;

	if (MemoryID.Index >= MAS_PAGE_MAX_COUNT)
		return;

	masPage* Page = &GMemory->PageList[MemoryID.Index];
	if (!Page->Data || Page->Generation != MemoryID.Generation)
		return;

	memset(Page->Data, 0, Page->Size);
	Page->Generation++;
	if (Page->Generation == 0)
		Page->Generation = 1;

	GMemory->FreeIDList[GMemory->FreeIDCount++] = MemoryID.Index;
}

void* masECSMemoryFrame_Malloc(uint64_t Size)
{
	if (!GMemory)
		return NULL;

	if (GMemory->FrameAllocIdx + Size > MAS_FRAME_MEMORY_SIZE)
		return NULL;

	void* Data = MAS_PTR_OFFSET(void, GMemory->Frame, GMemory->FrameAllocIdx);
	GMemory->FrameAllocIdx += Size;

	return Data;
}

void masECSMemoryFrame_Reset()
{
	if (!GMemory)
		return;

	memset(GMemory->Frame, 0, MAS_FRAME_MEMORY_SIZE);
	GMemory->FrameAllocIdx = 0;
}