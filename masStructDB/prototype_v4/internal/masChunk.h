#pragma once

#include <stdint.h>


struct masChunkListID
{
	uint64_t ID;
};

bool           masChunkList_Init();
void           masChunkList_DeInit();
masChunkListID masChunkList_Create();
void           masChunkList_Free(masChunkListID ChunkID);