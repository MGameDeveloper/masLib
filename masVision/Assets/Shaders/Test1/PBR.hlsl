#include "masTextureDefs.h"


/**************************************************************************
*
***************************************************************************/
cbuffer Camera : register(b0)
{
    matrix Projection;
    matrix View;
};

cbuffer Transformation : register(b1)
{
    matrix World;
};

cbuffer Material : register(b2)
{
    float4 BaseColor;
	float  Roughness;
	float  Metallic;
	float  EmissiveIntensity;
	float  Opacity;
	float  Clearcoat;
	float  ClearcoatRoughness;
	float  Anisotropy;
	float  Sheen;
};

Texture2D    Textures[MAS_TEXTURE_COUNT] : register(t0);
SamplerState Samplers[MAS_SAMPLER_COUNT] : register(s0);


/**************************************************************************
*
***************************************************************************/
struct VSInput
{
    float3 Position : Position;
    float3 Normal   : Normal;
    float3 Tangent  : Tangent;
    float2 TexCoord : TextCoord;
    float4 Color    : Color;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
};


/**************************************************************************
*
***************************************************************************/
VSOutput VSMain(VSInput In)
{
    VSOutput Out = (VSOutput)0;
    
    matrix PV     = mul(Projection, View);
    Out.Position  = mul(PV, In.Position);
    Out.Position *= Sheen;

    return Out;
}

float4 PSMain(VSOutput In)
{

}

