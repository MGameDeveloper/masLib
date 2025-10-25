#pragma once

#include "../masHandle.h"

MAS_DECLARE_HANDLE(masShader);  // should be in its own api with all supported functions
MAS_DECLARE_HANDLE(masTexture); // should be in its own api with all supported functions
MAS_DECLARE_HANDLE(masSampler); // should be in its own api with all supported functions

/**************************************************************************************************
*
***************************************************************************************************/
MAS_DECLARE_HANDLE(masMaterial);


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


/**************************************************************************************************
*
***************************************************************************************************/
masMaterial masMaterial_Create      (const char* Name = nullptr);
void        masMaterial_Destroy     (masMaterial* Material);
void        masMaterial_SetShader   (masMaterial Material, masShader Shader);
void        masMaterial_SetTexture  (masMaterial Material, int32_t TextureID, masTexture Texture);
void        masMaterial_UnSetTexture(masMaterial Material, int32_t TextureID);
void        masMaterial_SetScalar   (masMaterial Material, int32_t ScalarID, float Scalar);
float       masMaterial_GetScalar   (masMaterial Material, int32_t ScalarID);
masTexture  masMaterial_GetTexture  (masMaterial Material, int32_t TextureID);
masShader   masMaterial_GetShader   (masMaterial Material); // should encapsulates the finding the equivalant shader or compile one based on its existing features



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