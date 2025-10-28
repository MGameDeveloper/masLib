#pragma once

#include "masRawPool.h"

template<typename ELEMENT_TYPE>
class masPool : public masRawPool
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

    ELEMENT_TYPE* GetElement(masHandle Handle)
    {
        ELEMENT_TYPE *Element   = (ELEMENT_TYPE*)masRawPool::GetElement(Handle);
        return Element;
    }
};