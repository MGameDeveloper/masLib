#pragma once


void* masMemory_Malloc(size_t InSize, const char* InFile, const char* InFunc, int InLine);
bool  masMemory_Realloc(void** InPtr, size_t InSize, const char* InFile, const char* InFunc, int InLine);
void  masMemory_Free(void* InPtr, const char* InFile, const char* InFunc, int InLine);

#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((unsigned char*)ptr) + (offset))
#define MAS_MALLOC(type, size)            (type*)masMemory_Malloc(size,                       __FILE__, __FUNCTION__, __LINE__)
#define MAS_MALLOC_N(type, count)         (type*)masMemory_Malloc(sizeof(type) * count,       __FILE__, __FUNCTION__, __LINE__)
#define MAS_REALLOC(type, ptr, size)      (type*)masMemory_Realloc((void**)&ptr, size,                __FILE__, __FUNCTION__, __LINE__)
#define MAS_REALLOC_N(type, ptr, count)   (type*)masMemory_Realloc(ptr, sizeof(type) * count, __FILE__, __FUNCTION__, __LINE__)
#define MAS_FREE(ptr)                            masMemory_Free(ptr,                          __FILE__, __FUNCTION__, __LINE__)