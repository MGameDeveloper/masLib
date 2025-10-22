#pragma once

#include "masShader.h"
#include "masTextureDefs.h"

struct masTexture
{
	masResourceDef Def;

	ID3D11Texture2D          *pTexture2D;
	ID3D11ShaderResourceView *pSRV;
};

struct masSampler
{
	masResourceDef Def;

	ID3D11SamplerState* Sampler;
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

struct masMaterial
{
	masResourceDef Def;

	masTexture         Textures[MAS_TEXTURE_COUNT];
	masSampler         pSamplers[MAS_TEXTURE_COUNT];
	masMaterialScalars Scalars;
	masShader          Shader;
};