#include "masArray.h"

#include <stdlib.h>
#include <string.h>

#define MAS_REGISTER_NAME_SIZE 128
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)

struct masSlot
{
	uint32_t DataIdx;
	uint16_t Version;
	uint16_t RefCount;
};

struct masArray
{
	uint8_t *Data;
	masSlot *Slots;
	int32_t *FreeIndices;
	int32_t  FreeCount;
	int32_t  AllocIdx;
	int32_t  Capacity;
	int32_t  UseCount;
	int32_t  ElementSize;
	int32_t  ResizeCounter;
	int8_t   RegisterID;
	char     RegisterName[MAS_REGISTER_NAME_SIZE];
};

masArray* masArray_Create(const char* RegisterName, int32_t ElementSize, int32_t Capacity)
{
	if (ElementSize <= 0 || Capacity <= 0)
		return NULL;

	uint64_t  DataSize    = ElementSize     * Capacity;
	uint64_t  SlotSize    = sizeof(masSlot) * Capacity;
	uint64_t  IndicesSize = sizeof(int32_t) * Capacity;
	uint64_t  MemorySize  = sizeof(masArray) + DataSize + IndicesSize;
	masArray *Array       = (masArray*)malloc(MemorySize);
	if (!Array)
		return NULL;

	memset(Array, 0, MemorySize);

	Array->Data        = MAS_PTR_OFFSET(uint8_t, Array,        sizeof(masArray));
	Array->Slots       = MAS_PTR_OFFSET(masSlot, Array->Data,  DataSize);
	Array->FreeIndices = MAS_PTR_OFFSET(int32_t, Array->Slots, SlotSize);
	Array->Capacity    = Capacity;
	Array->ElementSize = ElementSize;
	Array->AllocIdx    = 0;
	Array->FreeCount   = 0;
	Array->UseCount    = 0;
	Array->RegisterID  = -1;

	if (RegisterName)
	{
		uint64_t NameLen = strlen(RegisterName);
		if (NameLen >= MAS_REGISTER_NAME_SIZE)
			NameLen = MAS_REGISTER_NAME_SIZE - 1;
		memcpy(Array->RegisterName, RegisterName, NameLen);

		//Array->RegisterID = masArrayRegistery_Add(RegisterName, Array);
	}


	return Array;
}

void masArray_Destroy(masArray** ArrayPtr)
{
	if (!ArrayPtr || !(*ArrayPtr))
		return;
	
	masArray* Array = *ArrayPtr;
	//if (Array->RegisterID != -1)
	//	masArrayRegistery_Remove(Array->RegisterID);

	free(*ArrayPtr);
	*ArrayPtr = NULL;

}

int32_t masArray_Capacity(masArray* Array)
{
	if (!Array)
		return 0;
	return Array->Capacity;
}

int32_t masArray_Size(masArray* Array)
{
	if (!Array)
		return 0;
	return Array->UseCount;
}

int32_t masArray_RegisterID(masArray* Array)
{
	if (!Array)
		return -1;
	return Array->RegisterID;
}

int32_t masArray_ResizeCount(masArray* Array)
{
	if (!Array)
		return 0;
	return Array->ResizeCounter;
}

int32_t masArray_FreeCount(masArray* Array)
{
	if (!Array)
		return 0;
	return Array->FreeCount;
}

int32_t masArray_ElementSize(masArray* Array)
{
	if (!Array)
		return 0;
	return Array->ElementSize;
}

const char* masArray_RegsiterName(masArray* Array)
{
	if (!Array)
		return NULL;
	return Array->RegisterName;
}

const void* masArray_Element(masArray* Array, masHandle Handle)
{

}

void* masArray_Alloc(masArray* Array, int32_t* OutIdx)
{
	if (!Array)
		return { 0 };


	int32_t DataIdx = -1;
	if (Array->AllocIdx >= Array->Capacity)
	{
		if (Array->FreeCount <= 0)
			masInternal_Resize();
		else
		{
			DataIdx = Array->FreeIndices[--Array->FreeCount];
			Array->FreeIndices[Array->FreeCount + 1] = -1;
		}
	}

	if (DataIdx == -1)
		DataIdx = Array->AllocIdx++;

	masHandle Handle = masHandle_Alloc(Array, DataIdx);
	
	return Handle;
}

void masArray_Free(masArray* Array, int32_t Idx)
{

}