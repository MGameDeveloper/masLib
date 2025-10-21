#pragma once

#include "masResourceMap.h"
#include "Renderer/masTextureDefs.h"
#include "Renderer/masShader.h"


struct masTexture : private masResource
{
	ComPtr<ID3D11Texture2D>           pTexture2D;
	ComPtr<ID3D11ShaderResourceView>  pSRV;
	std::string                       Path;

	void Release() override;
};

struct masMaterialScalars
{
	float BaseColor[4];
	float Roughness;
	float Metallic;
	float EmissiveIntensity;
	float Opacity;
	float Clearcoat;
	float ClearcoatRoughness;
	float Anisotropy;
	float Sheen;
};

struct masMaterial : private masResource
{
	ComPtr<ID3D11SamplerState> Samplers[MAS_TEXTURE_COUNT];
	masTexture* Textures[MAS_TEXTURE_COUNT];
	std::string Name;
	masMaterialScalars Scalars;
	masShader* Shader;

	//float SubsurfaceAmount;
	//uint32_t Flags; // opque/transparent/clearcoat/subsurface and so on

	void Release() override;
};

struct masMesh : private masResource
{
	ComPtr<ID3D11Buffer> pVertices;
	ComPtr<ID3D11Buffer> pIndices;
	masMaterial* Material;
	std::string          Name;
	uint32_t             VertexCount;
	uint32_t             IndexCount;

	void Release() override;
};

struct masModel : private masResource
{
	std::vector<masMesh*> Meshes;
	std::string           Name;

	void Release() override;
};

masModel* masModel_Load(const char* Path);
void masModel_UnLoad(masModel** Model);
void masModel_Draw(masModel* Model);