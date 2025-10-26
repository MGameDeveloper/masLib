#pragma once

#include <stdint.h>

class masRawPool;

struct masPoolDef
{
    masRawPool* Pool;
    char Name[64];
};

struct masPoolRegistery
{
    static uint8_t     Add(const char* Name, masRawPool* Pool);
    static void        Remove(uint8_t ID);
    static uint8_t     Count();

    static masPoolDef *GetDefByID(uint8_t ID);
    static masPoolDef *GetDefByName(const char* Name);

    static masRawPool* GetPoolByID(uint8_t ID);
    static masRawPool* GetPoolByName(const char* Name);

    static uint8_t GetPoolID(const char* Name);
};