#pragma once

#include <stdint.h>


///////////////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////////////

// actual handles generated for outer code and gameplay logic
typedef union _masEntity
{
	uint64_t Signature;
	struct
	{
		uint32_t Idx;
		uint16_t Generation;
	};
} masEntity;

// For outer handles safty and validity
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

// Represents a single page[ 16kb ] of entity with specific components combination
typedef struct _masArchTypeChunk
{
    uint8_t   **ComponentArrays;
    masEntity  *Entities;
    uint16_t    EntityCapacity;
    uint16_t    EntityUsedCount;
} masArchTypeChunk;

// Represents all pages for an entity type that has a specific combination of components
typedef struct _masArchType
{
    masArchTypeChunk **Chunks; // realloc to expand pointer list internal data stays in chunkpool
    uint16_t          *ComponentsType;
    uint16_t           ChunkCapacity;
    uint16_t           ChunkUsedCount;
    uint8_t            ComponentCount;
} masArchType;

// Describe an archtype that is registerd to be search for or filtered by systems of interest
typedef struct _masArchTypeDesc
{
    masArchType *ArchType;
    uint64_t     ArchTypeHash;
    char         Name[128];
} masArchTypeDesc;


// The registery that hold all archtypes of the game
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

// as per system request for components it need to work with
typedef struct _masArchTypeFilterChunk
{
    masEntity  *Entities;
    uint8_t   **Components; // all requested components' pointer collected from all archtypes that have the requested data
    uint16_t    Count;
} masArchTypeFilterChunk;

typedef struct _masArchTypeFilter
{
    masArchTypeFilterChunk **FilterChunks;
    uint16_t *ComponentStrides;
    uint16_t *ComponentTypes;  
    uint16_t  ComponentCount;
};


///////////////////////////////////////////////////////////////////////////////////////
// FUNC PROTOTYPE
///////////////////////////////////////////////////////////////////////////////////////

#define MAS_PROFILE_BEGIN()
#define MAS_PROFILE_END()

struct masTransformComponent 
{ 
    float world[16];
    float positoin[3];
    float rotate[4];
    float scale[3];
};

struct masSceneComponent     
{ 
    uint16_t CacheParentTransformIdx;
    uint16_t CacheParentDepthIdx;
    uint16_t ThisDepth;
};

// masArchType A = { masTransformComponent, masSceneComponent, masRigidBodyComponent }
// masArchType B = { masTransformComponent, masSceneComponent, masAudioComponent }


// example of system that target SceneComponent to transform entity and maintain hierarchy 
// REQUIRES: Sorting by depth to ensure parent processed before children and so on
void masUpdateSceneTransformsB()
{
    MAS_PROFILE_BEGIN();

    //masArchTypeArray ArchTypeArray = MAS_ECS_FIND_ARCHTYPE(masSceneComponent);
    //if (ArchTypeArray->Count == 0)
    //    return;
    //
    //for (int32_t ArchTypeIdx = 0; ArchTypeIdx < ArchTypeArray->Count; ++ArchTypeIdx)
    //{
    //    masArchType* ArchType = ArchTypeArray.ArchTypes[ArchTypeIdx];
    //
    //    for (int32_t ChunkIdx = 0; ChunkIdx < ArchType->ChunkUsedCount; ++ChunkIdx)
    //    {
    //        masArchTypeChunk* Chunk = ArchType->Chunks[ChunkIdx];
    //        for (int32_t EntityIdx = 0; EntityIdx < Chunk->EntityUsedCount; ++EntityIdx)
    //        {
    //            masEntity              Entity    = Chunk->Entities[EntityIdx];
    //            masTransformComponent *Transform = MAS_ECS_GET_COMPONENT(masTransformComponent, EntityIdx, Chunk->ComponentArrays);
    //            masSceneComponent     *Scene     = MAS_ECS_GET_COMPONENT(masSceneComponent,     EntityIdx, Chunk->ComponentArrays);
    //
    //            // logic
    //        }
    //    }
    //}

    MAS_PROFILE_END();
}


// lives in its own 16kb pages ownes by SceneHierarchyFlatteningSystem
typedef struct _masFlatSceneHierarcy 
{ 
    /*Data*/ 
} masFlatSceneHierarchy;

masFlatSceneHierarchy* masFlattenSceneHierarchy()
{
    // Build Depth Sort Wavefront
    // Owns Entity Relation Memory
}

void masPropagationSceneTransform(masFlatSceneHierarchy* FlatScene)
{
    // Travers FlatScene
    // Calculate Transform
    // Write to ECS chunk and Scratch Cache
}

void masUpdateSceneHierarchy()
{
    MAS_PROFILE_BEGIN();

    masFlatSceneHierarchy* FlatScene = masFlattenSceneHierarchy();
    masPropagationSceneTransform(FlatScene);
    
    MAS_PROFILE_END();
}

////////////////////////////////////////////// REFACTORY V2 //////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Entity handle used by outer system and gameplay logic
///////////////////////////////////////////////////////////////////////////////////////
typedef union _masEntity
{
    uint64_t Signature;
    struct
    {
        uint32_t Idx;
        uint16_t Generation;
        uint16_t UnUsed;
    };
} masEntity;


///////////////////////////////////////////////////////////////////////////////////////
// Entity Mapper used to validate handles used by systems and gameplay logic 
//     to prevent any stale handle from being used
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masEntityMapper
{
    uint32_t EntityID;
    uint16_t Generation;
    uint16_t ArchType;
    uint16_t ChunkID;
} masEntityMapper;


///////////////////////////////////////////////////////////////////////////////////////
// Persistent Components for every entity
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masLocalTransformComponent
{
    float Position[4];
    float Rotation[4];
    float Scale[4];
} masLocalTransform;

typedef struct _masWorldTransformComponent
{
    float mat[16];
} masWorldTransform;

typedef struct _masSceneComponent
{
    masEntity Parent;
    masEntity FirstChild;
    masEntity Next;
    uint32_t  DepthLevel;
} masSceneComponent;


///////////////////////////////////////////////////////////////////////////////////////
// used to offset for next component in uint8_t* to get next component correctly
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masComponentInfo
{
    uint16_t Stride;
    uint16_t Type;
} masComponentInfo;


///////////////////////////////////////////////////////////////////////////////////////
// A single chunk in an archtype
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masArchTypeChunk
{
    masEntity          *Entities;
    masLocalTransform  *LocalTransforms;
    masWorldTransform  *WorldTransforms;
    masSceneComponent  *EntitiesHierarchy;
    uint8_t           **ComponentArrays;
    uint32_t            EntityCapacity;
    uint32_t            EntityUsedCount;
} masArchTypeChunk;

typedef struct _masArchType
{
    masArchTypeChunk **Chunks; //[ Pointers List ] to allocated pages each page is 16KB in size
    masComponentInfo  *ComponentsInfo;
    uint64_t ComponentsHash;
    uint32_t ComponentCount;
    uint32_t Capacity;
    uint32_t Count;
} masArchType;


///////////////////////////////////////////////////////////////////////////////////////
// if Scene hierarchy is changed add it here to get udpated 
//     is there a better way to implement this effeciently
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masUpdateSceneHierarchyCommand
{
    masEntity *Entities;
    uint32_t   Capacity;
    uint32_t   Count;
} masUpdateSceneHierarchyCommand;

// from copilot
typedef struct _masWorkList
{
    masArchTypeChunk *Chunk;
    uint32_t          Slot;
} masWorkList;

typedef struct _masArchTypeChunk
{
    uint8_t  *Block;
    uint32_t  EntityOffset;
    uint32_t  ComponentsTableOffset; // uint32_t *offsets = Block + ComponentTableOffset;
    uint32_t  ComponentCount;
    uint32_t  Capacity;
    uint32_t  UsedCount;
} masArchTypeChunk;



////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _masLocalTransformComponent
{
    float Position[4];
    float Rotation[4];
    float Scale[4];
} masLocalTransform;

typedef struct _masWorldTransformComponent
{
    float matrix[16];
} masWorldTransform;

typedef struct _masParentComponent
{
    masEntity EntityHandle;
} masParentComponent;

typedef struct _masChildrenComponent
{
    masEntity FirstChildHandle;
    uint32_t  Count;
} masChildrenComponent;