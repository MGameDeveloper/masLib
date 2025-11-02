#include "masHandlePool.h"
#include "masArray.h"


/*******************************************************************************************************************
*
********************************************************************************************************************/
static masArray *GHandleArray       = masArray_Create("GHandleArray",       sizeof(masHandle),       1024 * 10);
static masArray *GHandleMapperArray = masArray_Create("GHandleMapperArray", sizeof(masHandleMapper), 1024 * 10);


/*******************************************************************************************************************
*
********************************************************************************************************************/
masHandle masHandle_Alloc(masArray* Array, int32_t DataIdx)
{
	if (!GHandleArray || !GHandleMapperArray)
		return { 0 };

	int32_t          HandleIdx       = -1;
	int32_t          HandleMapperIdx = -1;
	masHandle       *Handle          = MAS_ARRAY_ALLOC(masHandle,       GHandleArray,       &HandleIdx);
	masHandleMapper *HandleMapper    = MAS_ARRAY_ALLOC(masHandleMapper, GHandleMapperArray, &HandleMapperIdx);

	if (Handle && HandleMapper)
	{
		HandleMapper->HandleIdx  = HandleIdx;
		HandleMapper->DataIdx    = DataIdx;
		HandleMapper->RefCounter = 1;
		if (HandleMapper->Version == 0)
			HandleMapper->Version = 1;

		Handle->SlotIdx     = HandleMapperIdx;
		Handle->PoolID      = masArray_RegisterID(Array);
		Handle->Version     = HandleMapper->Version;
		Handle->HasParent   = false;
		Handle->HasChildren = false;

		return *Handle;
	}

	if (Handle)
		MAS_ARRAY_FREE(GHandleArray, HandleIdx);

	if (HandleMapper)
		MAS_ARRAY_FREE(GHandleMapperArray, HandleMapperIdx);

	return { 0 };
}

void masHandle_Free(masArray* Array, masHandle* Handle)
{
	if (!Array || !Handle || Handle->Signiture == 0)
		return;
	if (masArray_RegisterID(Array) != Handle->PoolID)
		return;

	masHandleMapper* HandleMapper = masArray_Element(GHandleMapperArray, Handle->SlotIdx);
	if (!HandleMapper)
		return;
	if (HandleMapper->Version != Handle->Version)
		return;

	HandleMapper->RefCounter--;
	if (HandleMapper->RefCounter <= 0)
	{
		MAS_ARRAY_FREE(Array, HandleMapper->DataIdx);
		HandleMapper->RefCounter = 0;
		HandleMapper->DataIdx    = -1;
		HandleMapper->Version++;
		if (HandleMapper->Version == 0)
			HandleMapper->Version = 1;

		int32_t HandleMapperIdx = Handle->SlotIdx;
		MAS_ARRAY_FREE(GHandleArray,       HandleMapper->HandleIdx);
		MAS_ARRAY_FREE(GHandleMapperArray, HandleMapperIdx);
	}

	*Handle = { 0 };
}

void* masHandle_Data(masArray* Array, masHandle Handle)
{

}