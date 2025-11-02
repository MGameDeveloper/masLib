#pragma once

#include <stdint.h>

struct masArray;

masArray*   masArray_Create(const char* RegisterName, int32_t ElementSize, int32_t Capacity);
void        masArray_Destroy(masArray** ArrayPtr);
int32_t     masArray_Capacity(masArray* Array);
int32_t     masArray_Size(masArray* Array);
int32_t     masArray_RegisterID(masArray* Array);
int32_t     masArray_ResizeCount(masArray* Array);
int32_t     masArray_FreeCount(masArray* Array);
int32_t     masArray_ElementSize(masArray* Array);
const char* masArray_RegsiterName(masArray* Array);
const void* masArray_Element(masArray* Array, int32_t Idx);

// void masArray_CompactElements(masArray* Array);

/*
* TODO:
*     - Elements should not be accessed via raw index, they should be accessed via handle to avoid memory accessing issues ( access freeed blocks,
*         using stale handle, ...)...)
*/