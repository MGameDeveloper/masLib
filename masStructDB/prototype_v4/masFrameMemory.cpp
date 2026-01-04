#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "internal/masMemory.h"
#include "masFrameMemory.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_FRAME_MEMORY_SIZE (1024 * 4)
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
struct masFrameMemory
{
	uint8_t* Data;
	uint32_t Idx;
};

struct masFrameAllocator
{
	void* Data;
	size_t  Size;
	size_t  Idx;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
static masFrameMemory GFrameMem = { };


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masFrameMemory_Init()
{
	if (GFrameMem.Data != NULL)
		return true;

	void* Data = MAS_MALLOC(void, MAS_FRAME_MEMORY_SIZE);
	if (!Data)
		return false;
	memset(Data, 0, MAS_FRAME_MEMORY_SIZE);

	GFrameMem.Data = MAS_PTR_OFFSET(uint8_t, Data, 0);
	GFrameMem.Idx  = 0;

	return true;
}

void  masFrameMemory_DeInit()
{
	MAS_FREE(GFrameMem.Data);
	memset(&GFrameMem, 0, sizeof(masFrameMemory));
}

void  masFrameMemory_Reset()
{
	if (!GFrameMem.Data)
		return;

	memset(GFrameMem.Data, 0, MAS_FRAME_MEMORY_SIZE);
	GFrameMem.Idx = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
masFrameAllocator* masFrameAllocator_Create(size_t InSize)
{
	if (!GFrameMem.Data)
		return NULL;

	size_t MemSize = sizeof(masFrameAllocator) + InSize;
	if (GFrameMem.Idx + MemSize >= MAS_FRAME_MEMORY_SIZE)
		return NULL;

	masFrameAllocator* Allocator = MAS_PTR_OFFSET(masFrameAllocator, GFrameMem.Data, GFrameMem.Idx);
	Allocator->Data              = MAS_PTR_OFFSET(void, Allocator, sizeof(masFrameAllocator));
	Allocator->Size              = InSize;
	Allocator->Idx               = 0;

	GFrameMem.Idx += MemSize;
	return Allocator;
}

void* masFrameAllocator_Malloc(masFrameAllocator* Allocator, size_t InSize, const char* InFile, const char* InFunc, int InLine)
{
	if (!Allocator)
		return NULL;

	if (Allocator->Idx + InSize >= Allocator->Size)
	{
		printf("[ ERROR ]: FRAME_MEMORY_ALLOCATOR[ %llu ]: NOT_ENOUGH_SPACE\n\t[ %s ] ( %d ) %s\n", InSize, InFunc, InLine, InFile);
		return NULL;
	}

	void *Data      = MAS_PTR_OFFSET(void, Allocator->Data, Allocator->Idx);
	Allocator->Idx += InSize;

	printf("[ TRACE ]: FRAME_MEMORY_ALLOCATOR[ %llu BYTE]:\n\t[ %s ] ( %d ) %s\n", InSize, InFunc, InLine, InFile);

	return Data;
}