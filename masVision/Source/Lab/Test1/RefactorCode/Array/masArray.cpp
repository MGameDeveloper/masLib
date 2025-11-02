#include "masArray.h"

#include <stdlib.h>
#include <string.h>

#define MAS_REGISTER_NAME_SIZE 128
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)

struct masArray
{
	uint8_t *Data;
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
	uint64_t  IndicesSize = sizeof(int32_t) * Capacity;
	uint64_t  MemorySize  = sizeof(masArray) + DataSize + IndicesSize;
	masArray *Array       = (masArray*)malloc(MemorySize);
	if (!Array)
		return NULL;

	memset(Array, 0, MemorySize);

	Array->Data        = MAS_PTR_OFFSET(uint8_t, Array,       sizeof(masArray));
	Array->FreeIndices = MAS_PTR_OFFSET(int32_t, Array->Data, DataSize);
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

const void* masArray_Element(masArray* Array, int32_t Idx)
{
	if (!Array || Idx < 0)
		return NULL;
	const void* Data = MAS_PTR_OFFSET(const void, Array->Data, Array->ElementSize * Idx);
	return Data;
}