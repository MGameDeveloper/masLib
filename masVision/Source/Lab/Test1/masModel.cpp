#include "masModel.h"

#include "Debug/Assert/masAssert.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#pragma comment(lib, "assimp-vc143-mt.lib")


/*****************************************************************************************************
*
******************************************************************************************************/
struct VertexFmt
{
	float Position[3];
	float Normal[3];
	float Tangent[3];
	float TexCoord[2];
	uint32_t Color;
};

struct masMaterial
{
	
};

struct masMesh
{
	uint32_t Offset;
	uint32_t VertexCount;
	uint32_t IndexCount;
	uint32_t MaterialIdx;
};

struct masModel
{
	masMaterial *Materials;
	masMesh     *Meshes;
	VertexFmt   *Vertices;
	uint32_t    *Indices;
	uint32_t     MeshCount;
	uint32_t     MaterialCount;
};


/*****************************************************************************************************
*
******************************************************************************************************/
masModel* masModel_Load(const char* Path)
{
	static Assimp::Importer Importer;

	uint32_t ImportFlags = aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded;
	const aiScene* Scene = Importer.ReadFile(Path, ImportFlags);
	MAS_ASSERT(Scene, "IMPORT_MODEL: %s", Path);

	for (uint32_t i; Scene->mNumMeshes; ++i)
	{
		const aiMesh* Mesh = Scene->mMeshes[i];
		Mesh->
	}


	Importer.FreeScene();
}

void masModel_UnLoad(masModel** Model)
{

}