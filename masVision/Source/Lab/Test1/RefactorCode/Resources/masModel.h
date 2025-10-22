#pragma once

#include <d3d11.h>

#include "masTransform.h"
#include "masResourceDef.h"

struct masMaterial;

struct masMesh
{
	masResourceDef Def;

	masTransform  Transform;
	ID3D11Buffer *Vertices;
	ID3D11Buffer *Indices;
	masMaterial  *Material;
	uint32_t      IndexCount;
};

struct masModel
{
	masResourceDef Def;

	masTransform   Transform;
	masMesh      **Meshes;
	uint32_t       MeshCount;
};