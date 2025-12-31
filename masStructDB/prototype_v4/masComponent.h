#pragma once

#include <stdint.h>

struct masComponent;

bool                masComponent_Init();
void                masComponent_DeInit();
void                masComponent_RegisterByName(const char* Name, uint32_t Size);
const masComponent* masComponent_Find(const char* Name);

#define masComponent_Register(Component)\
    masComponent_RegisterByName(#Component, sizeof(Component))

#define masComponentList(ListName, ...)\
    const char* ListName = #__VA_ARGS__
