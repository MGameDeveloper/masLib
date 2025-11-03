#include "masArray.h"

#include <stdlib.h>
#include <string.h>


/*************************************************************************************************************
*
**************************************************************************************************************/
#define MAS_REGISTER_NAME_SIZE 128
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)


/*************************************************************************************************************
*
**************************************************************************************************************/
struct masMapper
{
	uint32_t DataIdx;
	uint16_t Version;
	uint16_t RefCount;
};

struct masArray
{
	uint8_t   *Data;
	masMapper *Mappers;
	int32_t   *FreeIndices;
	int32_t    FreeCount;
	int32_t    AllocIdx;
	int32_t    Capacity;
	int32_t    UseCount;
	int32_t    ElementSize;
	int32_t    ResizeCounter;
	int8_t     RegisterID;
	char       RegisterName[MAS_REGISTER_NAME_SIZE];
};


/*************************************************************************************************************
*
**************************************************************************************************************/
bool masInternal_Resize(masArray** ArrayPtr)
{
	if (!ArrayPtr || !(*ArrayPtr))
		return false;

	masArray *Array    = *ArrayPtr;
	int32_t   Capacity = Array->Capacity * 2;

	uint64_t  DataSize    = Array->ElementSize * Capacity;
	uint64_t  MappersSize = sizeof(masMapper) * Capacity;
	uint64_t  IndicesSize = sizeof(int32_t) * Capacity;
	uint64_t  MemorySize  = sizeof(masArray) + DataSize + IndicesSize;
	masArray* LocalArray  = (masArray*)malloc(MemorySize);
	if (!LocalArray)
		return false;

	memset(LocalArray, 0, MemorySize);

	LocalArray->Data        = MAS_PTR_OFFSET(uint8_t,   Array,          sizeof(masArray));
	LocalArray->Mappers     = MAS_PTR_OFFSET(masMapper, Array->Data,    DataSize);
	LocalArray->FreeIndices = MAS_PTR_OFFSET(int32_t,   Array->Mappers, MappersSize);
	LocalArray->Capacity    = Capacity;
	LocalArray->ElementSize = Array->ElementSize;
	LocalArray->AllocIdx    = Array->AllocIdx;
	LocalArray->FreeCount   = Array->FreeCount;
	LocalArray->UseCount    = Array->UseCount;
	LocalArray->RegisterID  = Array->RegisterID;

	memcpy(LocalArray->Data,         Array->Data,         Array->ElementSize * Array->Capacity);
	memcpy(LocalArray->Mappers,      Array->Mappers,      sizeof(masMapper)  * Array->Capacity);
	memcpy(LocalArray->FreeIndices,  Array->FreeIndices,  sizeof(int32_t)    * Array->Capacity);
	memcpy(LocalArray->RegisterName, Array->RegisterName, strlen(Array->RegisterName));

	free(*ArrayPtr);
	*ArrayPtr = LocalArray;

	// masArrayRegistery_Update(LocalArray->RegisteredID, LocalArray);

	return true;
}


/*************************************************************************************************************
*
**************************************************************************************************************/
masArray* masArray_Create(const char* RegisterName, int32_t ElementSize, int32_t Capacity)
{
	if (ElementSize <= 0 || Capacity <= 0)
		return NULL;

	uint64_t  DataSize    = ElementSize       * Capacity;
	uint64_t  MappersSize = sizeof(masMapper) * Capacity;
	uint64_t  IndicesSize = sizeof(int32_t)   * Capacity;
	uint64_t  MemorySize  = sizeof(masArray) + DataSize + IndicesSize;
	masArray *Array       = (masArray*)malloc(MemorySize);
	if (!Array)
		return NULL;

	memset(Array, 0, MemorySize);

	Array->Data        = MAS_PTR_OFFSET(uint8_t,   Array,          sizeof(masArray));
	Array->Mappers     = MAS_PTR_OFFSET(masMapper, Array->Data,    DataSize);
	Array->FreeIndices = MAS_PTR_OFFSET(int32_t,   Array->Mappers, MappersSize);
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

void* masArray_Element(masArray* Array, masHandle Handle)
{
	if (!Array || (Array->RegisterID != Handle.PoolID) || Handle.Signiture == 0)
		return NULL;

	masMapper* Mapper = &Array->Mappers[Handle.MapperIdx];

	void* Data = MAS_PTR_OFFSET(void, Array->Data, Array->ElementSize * Mapper->DataIdx);

	return Data;
}

masHandle masArray_Alloc(masArray** ArrayPtr)
{
	if (!ArrayPtr || !(*ArrayPtr))
		return { 0 };

	masArray* Array = *ArrayPtr;

	int32_t MapperIdx = -1;
	if (Array->AllocIdx >= Array->Capacity)
	{
		if (Array->FreeCount <= 0)
		{
			if (masInternal_Resize(ArrayPtr))
				Array = *ArrayPtr;
			else
				return { 0 };
		}
		else
		{
			MapperIdx = Array->FreeIndices[--Array->FreeCount];
			Array->FreeIndices[Array->FreeCount + 1] = -1;
		}
	}

	if (MapperIdx == -1)
		MapperIdx = Array->AllocIdx++;

	masMapper* Mapper = &Array->Mappers[MapperIdx];
	if (Mapper->Version == 0)
		Mapper->Version = 1;
	Mapper->RefCount = 1;
	Mapper->DataIdx = MapperIdx; // CAUTION: data memory my be compacted to ensure its contigous upon processing so mappers indices to data should have specific logic to avoid bugs

	masHandle Handle = { 0 };
	Handle.PoolID    = Array->RegisterID;
	Handle.MapperIdx = MapperIdx;
	Handle.Version   = Mapper->Version;

	Array->UseCount++;

	return Handle;
}

void masArray_Free(masArray* Array, masHandle* Handle)
{
	if (!Array || !Handle || (Array->RegisterID != Handle->PoolID) || Handle->Signiture == 0)
		return;

	masMapper* Mapper = &Array->Mappers[Handle->MapperIdx];
	Mapper->RefCount--;
	if (Mapper->RefCount <= 0)
	{
		Mapper->RefCount = 0;
		Mapper->Version++;
		if (Mapper->Version == 0)
			Mapper->Version = 1;
		
		// To clean any internal resource of data that the user aware of
		//void* Data = MAS_PTR_OFFSET(void, Array->Data, Array->ElementSize * Mapper->DataIdx);
		//if (Array->DataCleanFunc)
		//	Array->DataCleanFunc(Data);

		Array->FreeIndices[Array->FreeCount++] = Handle->MapperIdx;

		Array->UseCount--;
	}

	*Handle = { 0 };
}