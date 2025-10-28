#pragma once

#include "../masHandle.h"
#include "../../Resources/masTextureDefs.h"


/**************************************************************************************************
*
***************************************************************************************************/
MAS_DECLARE_HANDLE(masShader);  // should be in its own api with all supported functions
MAS_DECLARE_HANDLE(masTexture); // should be in its own api with all supported functions
MAS_DECLARE_HANDLE(masSampler); // should be in its own api with all supported functions


/**************************************************************************************************
*
***************************************************************************************************/
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

struct masMaterialData
{
	masMaterialScalars Scalars;
	masTexture         Textures[MAS_TEXTURE_COUNT];
	masSampler         pSamplers[MAS_TEXTURE_COUNT];
	masShader          Shader;
};


/**************************************************************************************************
*
***************************************************************************************************/
class masMaterial
{
private:
	masHandle Handle;

public:
	masMaterial(const char* Name);
	~masMaterial();
	masMaterial(const masMaterial& Other);
	masMaterial& operator=(const masMaterial& Other);
	masMaterial(masMaterial&& Other);
	masMaterial& operator=(masMaterial&& Other);

	void SetTexture  (int32_t TextureID, const masTexture& Texture);
	void UnSetTexture(int32_t TextureID);
	void SetScalar   (int32_t ScalarID, float Scalar);
	void SetShader(masShader Shader);
	
	masTexture GetTexture(int32_t TextureID);
	masShader  GetShader (int32_t Shader);
	float      GetScalar (int32_t ScalarID);

};


/*
* Material Flags
*     - Alpha blending 
*     - Double Sided Rendering
*     - Emissive Toggle
*     - Streaming State
* 
* Texture ID: Define enums
*     - BaseColor
*     - Normal
*     - ...
* 
* Shader Permutation: Automatically select shader variant base on: [ masMaterial_ResolveShader() & masMaterial_ShaderVariant() ]
*     - Which textures are bound
*     - Which scalars are set
*     - Material Flags
* 
* Draw submission intergration: 
*     - Resolve Shader
*     - Bind Textures and scalars
*     - Submit draw with correct pipeline state
* 
* Descriptor Binding (DX11/12)
*     - Map TextureID to shader register (e.g t0, t1)
*     - Use shader reflection or fixed layout
*     - Bind textures via PSSetShaderResource() (DX11) or descriptor tables (DX12)
* 
* 
*/