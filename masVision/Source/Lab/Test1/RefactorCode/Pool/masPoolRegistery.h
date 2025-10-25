#pragma once

#include <stdint.h>

/*
* IDEA FROM COPILOT TO ACHIEVE POOL REGISTERY
*/
typedef struct masPoolDef {
    const char* Name;
    uint32_t *Capacity;
    uint32_t *UsedCount;
    void* PoolPtr;
    void* (*GetData)(void* pool, int32_t slotIdx);
    uint32_t  ElementSize;
    /*
    *  GetData is a function pointer that abstracts access to the pool’s internal data
    *       — enabling reflection and serialization without knowing the type
    */
} masPoolDef;