#pragma once

#include "masRawPool.h"

template<typename ELEMENT_TYPE, typename HANDLE_TYPE>
class masPool : masRawPool
{
public:
    masPool(const char* Name = nullptr) :
        masRawPool(Name, sizeof(ELEMENT_TYPE))
    { }

    ~masPool()
    { }

    masPool(const masPool& Other)            = delete; // Copy Semantic
    masPool& operator=(const masPool& Other) = delete; // Copy Semantic
    masPool(masPool&& Other)                 = delete; // Move Semantic
    masPool& operator=(masPool&& Other)      = delete; // Move Semantic

    ELEMENT_TYPE* GetElement(HANDLE_TYPE Handle)
    {
        masHandle     RawHandle = { Handle.SlotIdx, Handle.GenID };
        ELEMENT_TYPE *Element   = (ELEMENT_TYPE*)masRawPool::GetElement(RawHandle);
        return Element;
    }

    HANDLE_TYPE Alloc()
    {
        masHandle   RawHandle = masRawPool::Alloc();
        HANDLE_TYPE Handle    = { RawHandle.SlotIdx, RawHandle.GenID };
        return Handle;
    }

    void Free(HANDLE_TYPE* Handle)
    {
        masRawPool::Free((masHandle*)Handle);
    }

    bool IsEmpty()
    {
        return true;
    }
};