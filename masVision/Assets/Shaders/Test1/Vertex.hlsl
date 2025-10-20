
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

};

VSOutput Main(VSInput In)
{
    VSOutput Out = (VSOutput)0;

    return Out;
}

