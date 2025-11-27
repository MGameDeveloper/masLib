#pragma once

#include <stdint.h>

typedef uint32_t masECSMemoryID;

bool           masECSMemory_Init();
void           masECSMemory_DeInit();
masECSMemoryID masECSMemory_AllocPage();
void           masECSMemory_FreePage(masECSMemoryID ID);
void*          masECSMemoryFrame_Malloc(uint64_t Size);
void           masECSMemoryFrame_Reset();


#define MAS_ECS_FRAME_MALLOC(type, size) (type*)masECSMemoryFrame_Malloc(size)