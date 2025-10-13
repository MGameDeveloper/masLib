#pragma once

struct masTexture;

struct masMaterial
{
	masTexture* AlbedoMap;
	masTexture* NormalMap;
	masTexture* RoughnessMap;
	masTexture* MetallicMap;
	masTexture* EmissiveMap;
	masTexture* AmbientOcclusionMap;
	masTexture* OpacityMap;
	masTexture* ClearcoatMap;
	masTexture* AnisotropyMap;
	masTexture* SheenMap;
	masTexture* SubsurfaceMap;

	float BaseColor[4];
	float Roughness;
	float Metallic;
	float EmissiveIntensity;
	float Opacity;
	float Clearcoat;
	float ClearcoatRoughness;
	float Anisotropy;
	float SubsurfaceAmount;
};