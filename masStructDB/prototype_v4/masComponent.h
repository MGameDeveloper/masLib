#pragma once

#include <stdint.h>

struct masComponent
{
    uint64_t Hash;
    uint32_t UniqueID;
    uint32_t Size;
};

struct masComponentList
{
    char         *Names;
    masComponent *Components;
    uint32_t      Count;
    uint32_t      NameLen;
};

bool         masComponent_Init();
void         masComponent_DeInit();
void         masComponent_RegisterByName(const char* Name, uint32_t Size);
masComponent masComponent_Find(const char* Name);
bool         masComponent_IsValid(masComponent Component);
const masComponentList* masComponent_Query(const char* Components);

#define MAS_COMPONENT_REGISTER(Component)\
    masComponent_RegisterByName(#Component, sizeof(Component))

#define MAS_DECLARE_COMPONENTS(ListName, ...)\
    const char* ListName = #__VA_ARGS__
