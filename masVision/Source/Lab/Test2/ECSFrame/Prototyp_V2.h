#pragma once

#include <stdint.h>


///////////////////////////////////////////////////////////////////////////////////////
// Entity Handle and a way to prevent using stale handle by checking generation
//     from Mapper against handle's
///////////////////////////////////////////////////////////////////////////////////////
typedef union _masEntity
{
	uint64_t Handle;
	struct
	{
		uint32_t MapperIndex;
		uint16_t Generation;
		uint16_t UnUsed;
	};
} masEntity;

typedef struct _masEntityMapper
{
	uint16_t Generation;
	uint32_t ArchType;
	uint16_t Chunk;
	uint16_t IndexInChunk;
} masEntityMapper;


///////////////////////////////////////////////////////////////////////////////////////
// Persistent Components for entity to be placed in the scene 
//     -> [ masPositioni masScale, masEuler, masMatrix, masSceneNode ]
///////////////////////////////////////////////////////////////////////////////////////
typedef union _masVec3
{
	float xyz[3];
	struct
	{
		float x, y, z;
	};
} masVec3, masPosition, masScale, masEuler;

typedef union _masVec4
{
	float xyzw[4];
	struct
	{
		float x, y, z, w;
	};
} masVec4, masQuaternion;

typedef union _masMatrix
{
	float m16[16];
} masMatrix;

typedef struct _masSceneNode
{
	masEntity Parent;
	masEntity Child;
	masEntity Next;
	masEntity Prev;
} masSceneNode;


///////////////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masArchTypeChunk
{
	masPosition *Positions;
	masEuler    *Rotations;
	masScale    *Scales;
	masMatrix   *Matrices;

} masArchTypeChunk;