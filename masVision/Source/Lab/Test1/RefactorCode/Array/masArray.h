#pragma once

#include <stdint.h>

struct masArray;

masArray*   masArray_Create(const char* Name, int32_t ElementSize, int32_t Capacity);
void        masArray_Destroy(masArray** ArrayPtr);
int32_t     masArray_Capacity(masArray* Array);
int32_t     masArray_Size(masArray* Array);
int32_t     masArray_RegisterID(masArray* Array);
int32_t     masArray_ResizeCount(masArray* Array);
int32_t     masArray_FreeCount(masArray* Array);
int32_t     masArray_ElementSize(masArray* Array);
const char* masArray_Name(masArray* Array);
const void* masArray_Element(masArray* Array, int32_t Idx);

// void masArray_CompactElements(masArray* Array);