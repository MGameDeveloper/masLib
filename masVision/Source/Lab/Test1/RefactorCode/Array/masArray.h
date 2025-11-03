#pragma once

#include "masHandle.h"


/*************************************************************************************************************
*
**************************************************************************************************************/
struct masArray;


/*************************************************************************************************************
*
**************************************************************************************************************/
masArray*   masArray_Create(const char* RegisterName, int32_t ElementSize, int32_t Capacity);
void        masArray_Destroy(masArray** ArrayPtr);
int32_t     masArray_Capacity(masArray* Array);
int32_t     masArray_Size(masArray* Array);
int32_t     masArray_RegisterID(masArray* Array);
int32_t     masArray_ResizeCount(masArray* Array);
int32_t     masArray_FreeCount(masArray* Array);
int32_t     masArray_ElementSize(masArray* Array);
const char* masArray_RegsiterName(masArray* Array);
void*       masArray_Element(masArray* Array, masHandle Handle);
masHandle   masArray_Alloc(masArray** ArrayPtr);
void        masArray_Free(masArray* Array, masHandle* Handle);
// void masArray_CompactElements(masArray* Array);


/*************************************************************************************************************
*
**************************************************************************************************************/
#define MAS_ARRAY_ELEMENT(Type, Array, Handle) (Type*)masArray_Element(Array, Handle)


