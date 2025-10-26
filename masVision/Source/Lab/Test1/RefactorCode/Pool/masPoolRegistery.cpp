#include "masPoolRegistery.h"

#include <string.h>


/*****************************************************************************
*
******************************************************************************/
#define MAS_POOL_REGISTERY_SIZE 60
#define MAS_POOL_NAME_SIZE      64


/*****************************************************************************
*
******************************************************************************/
static masPoolDef GPoolDefs[MAS_POOL_REGISTERY_SIZE] = { };
static uint32_t   GPoolIdx = 1;


/*****************************************************************************
*
******************************************************************************/
static bool masInternal_IsValid(uint8_t ID)
{
    return (ID > 0 && ID < GPoolIdx && GPoolDefs[ID].Pool != nullptr);
}


/*****************************************************************************
*
******************************************************************************/
uint8_t masPoolRegistery::Add(const char* Name, masRawPool* Pool)
{
    if (!Name || !Pool || (GPoolIdx >= MAS_POOL_REGISTERY_SIZE))
        return 0;

    for (int32_t i = 0; i < GPoolIdx; ++i)
        if (strcmp(Name, GPoolDefs[i].Name) == 0)
            return 0;

    uint8_t     ID  = GPoolIdx++;
    masPoolDef *Def = &GPoolDefs[ID];

    uint64_t NameLen = strlen(Name);
    if (NameLen >= MAS_POOL_NAME_SIZE)
        NameLen = MAS_POOL_NAME_SIZE - 1;
    memcpy(Def->Name, Name, NameLen);

    return ID;
}

void masPoolRegistery::Remove(uint8_t ID)
{
    if (ID == 0 || ID >= GPoolIdx)
        return;

    GPoolDefs[ID].Pool = nullptr;
}

uint8_t masPoolRegistery::Count()
{
    if (GPoolIdx == 1)
        return 0;

    return GPoolIdx;
}

masPoolDef* masPoolRegistery::GetDefByID(uint8_t ID)
{
    if (!masInternal_IsValid(ID))
        return nullptr;
    return &GPoolDefs[ID];
}

masPoolDef* masPoolRegistery::GetDefByName(const char* Name)
{
    for (int32_t i = 1; i < GPoolIdx; ++i)
        if (strcmp(GPoolDefs[i].Name, Name) == 0)
            return &GPoolDefs[i];
    return nullptr;
}

masRawPool* masPoolRegistery::GetPoolByID(uint8_t ID)
{
    if (!masInternal_IsValid(ID))
        return nullptr;
    return GPoolDefs[ID].Pool;
}

masRawPool* masPoolRegistery::GetPoolByName(const char* Name)
{
    masPoolDef* Def = GetDefByName(Name);
    if (Def && Def->Pool)
        return Def->Pool;
    return nullptr;
}

uint8_t masPoolRegistery::GetPoolID(const char* Name)
{
    for (int32_t i = 1; i < GPoolIdx; ++i)
    {
        masPoolDef* Def = &GPoolDefs[i];
        if (strcmp(Def->Name, Name) == 0)
        {
            if (Def->Pool)
                return i;
            else
                return 0;
        }
    }
           
    return 0;
}