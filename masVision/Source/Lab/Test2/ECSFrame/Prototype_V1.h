#pragma once

#include <stdint.h>


///////////////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////////////
typedef union _masEntity
{
	uint64_t Signature;
	struct
	{
		uint32_t Idx;
		uint16_t Generation;
	};
} masEntity;

typedef struct _masEntityMapper
{
    uint32_t EntityID;
    uint16_t Generation;
    uint16_t ArchType;
    uint16_t ChunkID;
} masEntityMapper;


///////////////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masArchTypeChunk
{
    uint8_t   **ComponentArrays;
    masEntity  *Entities;
    uint16_t    EntityCapacity;
    uint16_t    EntityUsedCount;
} masArchTypeChunk;

typedef struct _masArchType
{
    masArchTypeChunk **Chunks; // realloc to expand pointer list internal data stays in chunkpool
    uint16_t          *ComponentsType;
    uint16_t           ChunkCapacity;
    uint16_t           ChunkUsedCount;
    uint8_t            ComponentCount;
} masArchType;

typedef struct _masArchTypeDesc
{
    masArchType *ArchType;
    uint64_t     ArchTypeHash;
    char         Name[128];
} masArchTypeDesc;

typedef struct _masArchTypeRegistery
{
    masArchTypeDesc **ArchTypes; // realloc to expand
    uint16_t          Count;
} masArchTypeRegistery;


///////////////////////////////////////////////////////////////////////////////////////
// TO BE ADDED
///////////////////////////////////////////////////////////////////////////////////////
typedef struct masArchTypeChunkPool
{
    // all pages requested by archtypes
} masArchTypeChunkPool;

typedef struct _masSystemDesc
{
    // to associate with archtype layout
} masSystemDesc;

typedef struct _masComponentRegistery
{
    // size and alignment
} masComponentRegistery;


///////////////////////////////////////////////////////////////////////////////////////
// FUNC PROTOTYPE
///////////////////////////////////////////////////////////////////////////////////////

struct masTransformComponent { /*data*/ };
struct masSceneComponent     { /*data*/ };

// example of system that target SceneComponent to transform entity and maintain hierarchy 
// REQUIRES: Sorting by depth to ensure parent processed before children and so on
void masUpdateSceneTransformsB()
{
    MAS_PROFILE_BEGIN();

    masArchTypeArray ArchTypeArray = MAS_ECS_FIND_ARCHTYPE(masSceneComponent);
    if (ArchTypeArray->Count == 0)
        return;

    for (int32_t ArchTypeIdx = 0; ArchTypeIdx < ArchTypeArray->Count; ++ArchTypeIdx)
    {
        masArchType* ArchType = ArchTypeArray.ArchTypes[ArchTypeIdx];

        for (int32_t ChunkIdx = 0; ChunkIdx < ArchType->ChunkUsedCount; ++ChunkIdx)
        {
            masArchTypeChunk* Chunk = ArchType->Chunks[ChunkIdx];
            for (int32_t EntityIdx = 0; EntityIdx < Chunk->EntityUsedCount; ++EntityIdx)
            {
                masEntity              Entity    = Chunk->Entities[EntityIdx];
                masTransformComponent *Transform = MAS_ECS_GET_COMPONENT(masTransformComponent, EntityIdx, Chunk->ComponentArrays);
                masSceneComponent     *Scene     = MAS_ECS_GET_COMPONENT(masSceneComponent,     EntityIdx, Chunk->ComponentArrays);

                // logic
            }
        }
    }

    MAS_PROFILE_END();
}