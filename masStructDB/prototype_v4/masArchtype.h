#pragma once

#include <stdint.h>

struct masArchtype;

bool masArchtype_Init();
void masArchtype_DeInit();


uint64_t masArchtype_CreateEntity(const char* Components);
void     masArchtype_DestroyEntity(uint64_t Entity);
void*    masArchtype_AddEntityComponent(uint64_t Entity, const char* Component);
void     masArchtype_RemoveEntityComponent(uint64_t Entity, const char* Component);
void*    masArchtype_GetEntityComponent(uint64_t Entity, const char* Component);
void     masArchtype_AddEntityChild(uint64_t Entity, uint64_t Child);
void     masArchtype_RemoveEntityChild(uint64_t Child);

