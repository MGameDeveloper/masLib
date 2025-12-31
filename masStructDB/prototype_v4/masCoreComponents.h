#pragma once

#include "masEntity.h"

typedef union masVec3
{
	float xyz[3];
	struct
	{
		float x, y, z;
	};
} masPosition, masRotation, masScale, masVelocity;

typedef union masVec4
{
	float xyzw[4];
	struct
	{
		float x, y, z, w;
	};
} masQuaternion;

typedef union masMatrix4x4
{
	float m16[16];
	float m4x4[4][4];
} masWorldMatrix, masLocalMatrix;

typedef struct masParent
{
	masEntity Entity;
	uint32_t  DepthLevel;
};

typedef struct masChildren
{
	uint32_t Index;
	uint32_t Count;
	uint32_t DepthLevel;
};