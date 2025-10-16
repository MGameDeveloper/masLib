#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#pragma comment(lib, "assimp-vc143-mt.lib")

#include "masModel.h"
#include "Debug/Assert/masAssert.h"
#include "Graphics/masGraphics.h"
#include "masAssetSearch.h"
#include "masResourceMap.h"


/*****************************************************************************************************
*
******************************************************************************************************/
struct masVertexFmt
{
	float Position[3];
	float Normal[3];
	float Tangent[3];
	float TexCoord[3];
	float Color[4];
};

enum masTextureType
{
	MAS_TEXTURE_UNKNOWN = -1,

	MAS_TEXTURE_BASE_COLOR,
	MAS_TEXTURE_NORMAL,
	MAS_TEXTURE_ROUGHNESS,
	MAS_TEXTURE_METALLIC,
	MAS_TEXTURE_EMISSIVE,
	MAS_TEXTURE_AMBIENT_OCCLUSION,
	MAS_TEXTURE_OPACITY,
	MAS_TEXTURE_CLEARCOAT,
	MAS_TEXTURE_ANISOTROPY,
	MAS_TEXTURE_SHEEN,
	MAS_TEXTURE_SUBSURFACE,

	MAS_TEXTURE_COUNT
};

struct masTexture : private masResource
{
	ComPtr<ID3D11Texture2D>           pTexture2D;
	ComPtr<ID3D11ShaderResourceView>  pSRV;
	std::string                       Path;

	void Release() override
	{
		pTexture2D->Release();
		pSRV->Release();
	}
};
static masResourceMap<masTexture> TextureMap;


struct masMaterial : private masResource
{
	masTexture* Textures[MAS_TEXTURE_COUNT];
	std::string Name;

	float BaseColor[4];
	float Roughness;
	float Metallic;
	float EmissiveIntensity;
	float Opacity;
	float Clearcoat;
	float ClearcoatRoughness;
	float Anisotropy;
	float Sheen;

	//float SubsurfaceAmount;
	//uint32_t Flags; // opque/transparent/clearcoat/subsurface and so on

	void Release() override 
	{ 
		for (int32_t i = 0; i < MAS_TEXTURE_COUNT; ++i)
			TextureMap.Destroy(&Textures[i]);
	}
};
static masResourceMap<masMaterial> MaterialMap;


struct masMesh : private masResource
{
	ComPtr<ID3D11Buffer> pVertices;
	ComPtr<ID3D11Buffer> pIndices;
	masMaterial         *Material;
	std::string          Name;
	uint32_t             VertexCount;
	uint32_t             IndexCount;

	void Release() override 
	{
		pVertices->Release();
		pIndices->Release();
		MaterialMap.Destroy(&Material);
	}
};
static masResourceMap<masMesh> MeshMap;


struct masModel : private masResource
{
	std::vector<masMesh*> Meshes;
	std::string           Name;

	void Release() override
	{
		for (int32_t i = 0; i < Meshes.size(); ++i)
			MeshMap.Destroy(&Meshes[i]);
	}
};
static masResourceMap<masModel> ModelMap;



/*****************************************************************************************************
* For Debugging
******************************************************************************************************/
const char* AssimpTextureTypeName(aiTextureType Type)
{
	static const char* Name[AI_TEXTURE_TYPE_MAX + 1] =
	{
		"aiTextureType_NONE",
		"aiTextureType_DIFFUSE",
		"aiTextureType_SPECULAR",
		"aiTextureType_AMBIENT",
		"aiTextureType_EMISSIVE",
		"aiTextureType_HEIGHT",
		"aiTextureType_NORMALS",
		"aiTextureType_SHININESS",
		"aiTextureType_OPACITY",
		"aiTextureType_DISPLACEMENT",
		"aiTextureType_LIGHTMAP",
		"aiTextureType_REFLECTION",
		"aiTextureType_BASE_COLOR",
		"aiTextureType_NORMAL_CAMERA",
		"aiTextureType_EMISSION_COLOR",
		"aiTextureType_METALNESS",
		"aiTextureType_DIFFUSE_ROUGHNESS",
		"aiTextureType_AMBIENT_OCCLUSION",
		"aiTextureType_UNKNOWN",
		"aiTextureType_SHEEN",
		"aiTextureType_CLEARCOAT",
		"aiTextureType_TRANSMISSION",
		"aiTextureType_MAYA_BASE",
		"aiTextureType_MAYA_SPECULAR",
		"aiTextureType_MAYA_SPECULAR_COLOR",
		"aiTextureType_MAYA_SPECULAR_ROUGHNESS",
		"aiTextureType_ANISOTROPY",
		"aiTextureType_GLTF_METALLIC_ROUGHNESS",
	};

	return Name[Type];
}


/***************************************************************************************************************************
*
****************************************************************************************************************************/
static masMesh* masModelInternal_LoadMesh_Geometry(const aiMesh* AIMesh, const masAssetSearch& ModelPath)
{
	masMesh* Mesh = MeshMap.Find(AIMesh->mName.C_Str());
	if (Mesh)
		return Mesh;

	std::vector<masVertexFmt> Vertices;
	std::vector<uint32_t>     Indices;

	// Vertices
	masVertexFmt Vrtx = { 0 };
	for (uint32_t v = 0; v < AIMesh->mNumVertices; ++v)
	{
		memcpy(Vrtx.Position, &AIMesh->mVertices[v], sizeof(aiVector3D));

		if (AIMesh->HasNormals())
			memcpy(Vrtx.Normal, &AIMesh->mNormals[v], sizeof(aiVector3D));

		if (AIMesh->HasTangentsAndBitangents())
		{
			//aiVector3D BiTang = AIMesh->mBitangents[v];
			memcpy(Vrtx.Tangent, &AIMesh->mTangents[v], sizeof(aiVector3D));
		}

		if (AIMesh->HasTextureCoords(0))	
			memcpy(Vrtx.TexCoord, &AIMesh->mTextureCoords[0][v], sizeof(aiVector3D));

		if (AIMesh->HasVertexColors(0))
			memcpy(Vrtx.Color, &AIMesh->mColors[0][v], sizeof(aiColor4D));

		Vertices.push_back(Vrtx);
	}

	// Indices Importing
	for (uint32_t f = 0; f < AIMesh->mNumFaces; ++f)
	{
		const aiFace* Face = &AIMesh->mFaces[f];
		MAS_ASSERT(Face->mNumIndices == 3, "ImportMesh: Mesh is not triangulated -> %s", ModelPath.Path().c_str());
		Indices.push_back(Face->mIndices[0]);
		Indices.push_back(Face->mIndices[1]);
		Indices.push_back(Face->mIndices[2]);
	}

	if (!Vertices.empty() && !Indices.empty())
	{
		Mesh              = MeshMap.Create(AIMesh->mName.C_Str());
		Mesh->VertexCount = (uint32_t)Vertices.size();
		Mesh->IndexCount  = (uint32_t)Indices.size();
		Mesh->Name        = AIMesh->mName.C_Str();
		Mesh->pVertices   = masGraphics_CreateVertexBuffer(Vertices.data(), Mesh->VertexCount, sizeof(masVertexFmt));
		Mesh->pIndices    = masGraphics_CreateIndexBuffer(Indices.data(), Mesh->IndexCount);
	}

	return Mesh;
}
static masTextureType masTextureTypeFromAssimp(aiTextureType Type)
{
	switch (Type)
	{
	case aiTextureType_BASE_COLOR:        return MAS_TEXTURE_BASE_COLOR;
	case aiTextureType_NORMALS:           return MAS_TEXTURE_NORMAL;
	case aiTextureType_DIFFUSE_ROUGHNESS: return MAS_TEXTURE_ROUGHNESS;
	case aiTextureType_METALNESS:         return MAS_TEXTURE_METALLIC;
	case aiTextureType_EMISSIVE:          return MAS_TEXTURE_EMISSIVE;
	case aiTextureType_LIGHTMAP:          return MAS_TEXTURE_AMBIENT_OCCLUSION;
	case aiTextureType_OPACITY:           return MAS_TEXTURE_OPACITY;
	case aiTextureType_CLEARCOAT:         return MAS_TEXTURE_CLEARCOAT;
	case aiTextureType_ANISOTROPY:        return MAS_TEXTURE_ANISOTROPY;
	case aiTextureType_SHEEN:             return MAS_TEXTURE_SHEEN;

	//case aiTextureType_surface: return MAS_TEXTURE_SUBSURFACE;
	}

	
	return MAS_TEXTURE_UNKNOWN;
}
static void masInternal_LoadTexture(const aiMaterial* AIMaterial, masMaterial* Material, const masAssetSearch& ModelPath, aiTextureType TextureType, uint32_t Index)
{
	aiString Path;
	if (aiReturn_FAILURE == AIMaterial->GetTexture(TextureType, Index, &Path))
		return;

	std::string LoadPath(ModelPath.Directory() + Path.C_Str());
	masTexture* Texture = TextureMap.Find(LoadPath);
	if (!Texture)
	{
		Texture = TextureMap.Create(LoadPath);
		if (!Texture)
		{
			printf("[ERROR]: Failed to load Texture -> %s\n", LoadPath.c_str());
			return;
		}

		Texture->pTexture2D = masGraphics_CreateTexture2D(LoadPath.c_str());
		Texture->pSRV       = masGraphics_CreateShaderResourceView_Texture2D(Texture->pTexture2D);
		Texture->Path       = LoadPath;
	}

	Material->Textures[masTextureTypeFromAssimp(TextureType)] = Texture;
}
static masMaterial* masModelInternal_LoadMesh_Material(const aiScene* AIScene, const aiMesh* AIMesh, const masAssetSearch& ModelPath)
{
	// get coeffecients & textures as single material
	const aiMaterial* Material = AIScene->mMaterials[AIMesh->mMaterialIndex];
	printf("Material: %s\n", Material->GetName().C_Str());

	masMaterial *masMat = MaterialMap.Find(Material->GetName().C_Str());
	if (masMat)
		return masMat;

	masMat = MaterialMap.Create(Material->GetName().C_Str());
	MAS_ASSERT(masMat, "MATERIAL_CREATION_FAILED: %s", ModelPath.Path().c_str());

	masMat->Name = Material->GetName().C_Str();
	Material->Get(AI_MATKEY_BASE_COLOR,                 masMat->BaseColor);
	Material->Get(AI_MATKEY_ROUGHNESS_FACTOR,           masMat->Roughness);
	Material->Get(AI_MATKEY_METALLIC_FACTOR,            masMat->Metallic);
	Material->Get(AI_MATKEY_EMISSIVE_INTENSITY,         masMat->EmissiveIntensity);
	Material->Get(AI_MATKEY_OPACITY,                    masMat->Opacity);
	Material->Get(AI_MATKEY_CLEARCOAT_FACTOR,           masMat->Clearcoat);
	Material->Get(AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, masMat->ClearcoatRoughness);
	Material->Get(AI_MATKEY_ANISOTROPY_FACTOR,          masMat->Anisotropy);
	Material->Get(AI_MATKEY_SHEEN_COLOR_FACTOR,         masMat->Sheen);
	// Material->Get(AI_MATKEY_) need to find a way to get subsurface amount 

    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_BASE_COLOR,        0);
    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_NORMALS,           0);
    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_DIFFUSE_ROUGHNESS, 0);
    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_METALNESS,         0);
    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_EMISSIVE,          0);
    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_LIGHTMAP,          0);
    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_OPACITY,           0);
    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_CLEARCOAT,         0);
    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_ANISOTROPY,        0);
    masInternal_LoadTexture(Material, masMat, ModelPath, aiTextureType_SHEEN,             0);

#if 0
	for (uint32_t t = 0; t < AI_TEXTURE_TYPE_MAX; ++t)
	{
		aiTextureType Type = (aiTextureType)t;
		for (uint32_t i = 0; i < Material->GetTextureCount(Type); ++i)
		{
			aiString TexturePath;
			if (Material->GetTexture(Type, i, &TexturePath) == aiReturn_FAILURE)
				continue;	

			masTextureType masType = masTextureTypeFromAssimp(Type);
			if (masType != MAS_TEXTURE_UNKNOWN)
			{
				std::string TextureLoadPath(ModelPath.Directory() + TexturePath.C_Str());
				masTexture* Texture = TextureMap.Find(TextureLoadPath);
				if (Texture)
					masMat->Textures[masType] = Texture;
				else
				{
					Texture = TextureMap.Create(TextureLoadPath);
					if(!Texture)
						printf("[ERROR]: Failed to load Texture -> %s\n", TextureLoadPath.c_str());

					Texture->pTexture2D = masGraphics_CreateTexture2D(TextureLoadPath.c_str());
					Texture->pSRV = masGraphics_CreateShaderResourceView_Texture2D(Texture->pTexture2D);
					Texture->Path = TextureLoadPath;

				}	
				printf("    %s -> %s\n", AssimpTextureTypeName(Type), TexturePath.C_Str());
			}
			else
				printf("[ERROR]: no mapping from %s to masTextureType\n", AssimpTextureTypeName(Type));			
		}
	}
#endif

	return masMat;
}
static masMesh* masModelInternal_LoadMesh(const aiScene* AIScene, const aiMesh* AIMesh, const masAssetSearch& ModelPath)
{
	MAS_ASSERT((AIMesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE), "ImportMesh: Only support mesh created of triangles -> %s", ModelPath.Path().c_str());

	masMesh* Mesh  = masModelInternal_LoadMesh_Geometry(AIMesh, ModelPath);
	Mesh->Material = masModelInternal_LoadMesh_Material(AIScene, AIMesh, ModelPath);

	return Mesh;
}


/***************************************************************************************************************************
*
****************************************************************************************************************************/
masModel* masModel_Load(const char* Path)
{
	masModel* Model = ModelMap.Find(Path);
	if (Model)
		return Model;
	
	static Assimp::Importer Importer;

	masAssetSearch ModelPath(Path, "scene.gltf");
	MAS_ASSERT(ModelPath.IsValid(), "AssetSearch[ %s ]: Couldn't be found", Path);

	uint32_t       ImportFlags = aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded;
	const aiScene * AIScene       = Importer.ReadFile(ModelPath.Path(), ImportFlags);
	MAS_ASSERT(AIScene, "IMPORT_MODEL: %s", Path);

	Model = ModelMap.Create(Path);
	MAS_ASSERT(Model, "MODEL_CREATE_FAILED: %s", Path);
	for (uint32_t i = 0; i < AIScene->mNumMeshes; ++i)
	{
		const aiMesh* AIMesh = AIScene->mMeshes[i];

		masMesh* Mesh = masModelInternal_LoadMesh(AIScene, AIMesh, ModelPath);
		if (Mesh)
			Model->Meshes.push_back(Mesh);
	}
	Model->Name = AIScene->mName.C_Str();


	Importer.FreeScene();
	return nullptr;
}

void masModel_UnLoad(masModel** Model)
{

}

void masModel_Draw(masModel* Model)
{
	const masD3D11* x = masGraphics_D3D11();

	//masSetTopology(MasterTopology);
	//masSetInputLayout(MasterInputLayout);
	//masSetShader(MasterShader);
	//masUpdateCameraData(CurrentCamera);
	//masUpdateMaterial(MasterMaterial);
	//
	//for (auto& Mesh : Model->Meshes)
	//{
	//	if (!Mesh || Mesh->IndexCount == 0)
	//		continue;
	//
	//	masBindVertexAndIndexBuffer(Mesh);	
	//	masUpdateTextures(Mesh);
	//	x->ImmediateContext->DrawIndexed(Mesh->IndexCount, 0, 0);
	//}
}
