#pragma once

#include "masHandle.h"

struct masArray;

struct masHandleMapper
{
	int32_t DataIdx;
	int32_t HandleIdx;
	int32_t Version;
	int32_t RefCounter;
};

masHandle masHandle_Alloc(masArray* Array, int32_t DataIdx);
void      masHandle_Free(masArray* Array, masHandle* Handle);
void* masHandle_Data(masArray* Array, masHandle Handle);
