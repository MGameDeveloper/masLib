#pragma once

#include <stdint.h>


struct masChunkListID
{
	uint64_t ID;
};

bool           masChunkList_Init();
void           masChunkList_DeInit();
masChunkListID masChunkList_Create();
void           masChunkList_Free(masChunkListID ChunkListID);
size_t         masChunkList_ChunkSize();
bool           masChunkList_IsValid(masChunkListID ChunkListID);