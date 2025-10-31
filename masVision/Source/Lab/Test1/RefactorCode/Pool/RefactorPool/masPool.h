#pragma once

#include "masHandle.h"

/***************************************************************************************************
*
****************************************************************************************************/
typedef void(*masPoolFunc_ReleaseItem)(void* Item);
typedef bool(*masPoolFunc_FindItem)(const void* Item, const void* Key);


/***************************************************************************************************
*
****************************************************************************************************/
#define MAS_POOL_RELEASE_ITEM_FUNC(Func) (masPoolFunc_ReleaseItem)&Func
#define MAS_POOL_FIND_ITEM_FUNC(Func)    (masPoolFunc_FindItem)&Func


/***************************************************************************************************
*
****************************************************************************************************/
struct masPool;


/***************************************************************************************************
*
****************************************************************************************************/
masPool*  masPool_Create       (const char* Name, int32_t ElementSize, masPoolFunc_ReleaseItem ReleaseItemFunc);
void      masPool_Destroy      (masPool** PoolPtr);
masHandle masPool_Alloc        (masPool* Pool);
void      masPool_Free         (masPool* Pool, masHandle *Handle);
void*     masPool_GetElement   (masPool* Pool, masHandle Handle);
masHandle masPool_Find         (masPool* Pool, masPoolFunc_FindItem FindItemFunc, const void* Key);
void      masPool_Clear        (masPool* Pool);
int32_t   masPool_Capacity     (masPool* Pool);
int32_t   masPool_Size         (masPool* Pool);
bool      masPool_IsEmpty      (masPool* Pool);
masHandle masPool_AddRef       (masPool* Pool, masHandle Handle);
masHandle masPool_StealHandle  (masPool* Pool, masHandle* Handle);
bool      masPool_IsHandleValid(masPool* Pool, masHandle Handle);


/***************************************************************************************************
*
****************************************************************************************************/
#define MAS_DEFINE_GLOBAL_POOL(Pool, HandleType, DataType)\
    HandleType Pool##_Alloc()                                                  { return *(HandleType*)&masPool_Alloc(Pool);                   }\
    void       Pool##_Free(HandleType *Handle)                                 { masPool_Free(Pool, (masHandle*)Handle);                      }\
    DataType*  Pool##_Get(HandleType Handle)                                   { return (DataType*)masPool_GetElement(Pool, Handle.Handle);   }\
    HandleType Pool##_Find(masPoolFunc_FindItem FindItemFunc, const void* Key) { return *(HandleType*)&masPool_Find(Pool, FindItemFunc, Key); }\
    void       Pool##_Clear()                                                  { masPool_Clear(Pool);                                         }\
    int32_t    Pool##_Capacity()                                               { return masPool_Capacity(Pool);                               }\
    int32_t    Pool##_Size()                                                   { return masPool_Size(Pool);                                   }\
    bool       Pool##_IsEmpty()                                                { return masPool_IsEmpty(Pool);                                }\
    void       Pool##_AddRef(HandleType Handle)                                { masPool_AddRef(Pool, Handle.Handle);                         }\
    bool       Pool##_IsHandleValid(HandleType Handle)                         { return masPool_IsHandleValid(Pool, Handle.Handle);           }


#define MAS_DEFINE_HANDLE(HandleType, Pool)\
    HandleType::HandleType()                                 {                                                                  }\
    HandleType::~HandleType()                                { masPool_Free(Pool, &Handle);                                     }\
    HandleType::HandleType(const HandleType& Other)          { Handle = masPool_AddRef(Pool, Other.Handle);                     }\
    HandleType::HandleType(HandleType&& Other)               { Handle = masPool_StealHandle(Pool, &Other.Handle);               }\
    HandleType& HandleType::operator=(const HandleType& Rhs) { Handle = masPool_AddRef(Pool, Rhs.Handle);       return *this;   }\
    HandleType& HandleType::operator=(HandleType&& Rhs)      { Handle = masPool_StealHandle(Pool, &Rhs.Handle); return *this;   }
