#include "masMaterialScalars.h"
#include "../masPool.h"




/*****************************************************************************
*
******************************************************************************/
struct masMaterialScalarData
{
	masBaseColor BaseColor;
	float        Roughness;
	float        Metallic;
	float        EmissiveIntensity;
	float        Opacity;
	float        Clearcoat;
	float        ClearcoatRoughness;
	float        Anisotropy;
	float        Sheen;
};


/*****************************************************************************
*
******************************************************************************/
static masPool<masMaterialScalarData> GMaterialScalars("MaterialScalars");


/*****************************************************************************
*
******************************************************************************/
#define MAS_MATERIAL_SET_SCALAR(Scalar)\
    masMaterialScalarData* Data = GMaterialScalars.GetElement(Handle);\
    if(Data) Data->Scalar = Scalar

#define MAS_MATERIAL_GET_SCALAR(Scalar, FailReturn)\
    masMaterialScalarData* Data = GMaterialScalars.GetElement(Handle);\
    if(Data)\
        return Data->Scalar;\
    return FailReturn


/*****************************************************************************
*
******************************************************************************/
masMaterialScalars::masMaterialScalars()
{
	Handle = GMaterialScalars.Alloc();
}

masMaterialScalars::~masMaterialScalars()
{
	GMaterialScalars.Free(Handle);
}

masMaterialScalars::masMaterialScalars(const masMaterialScalars& Other)
{
	Handle = Other.Handle;
	GMaterialScalars.AddRef(Handle);
}

masMaterialScalars::masMaterialScalars(masMaterialScalars&& Other)
{
	Handle = Other.Handle;
	Other.Handle = {};
}

masMaterialScalars& masMaterialScalars::operator=(const masMaterialScalars& Rhs)
{
	if (this != &Rhs)
	{
		GMaterialScalars.Free(Handle);
		Handle = Rhs.Handle;
		GMaterialScalars.AddRef(Handle);
	}

	return *this;
}

masMaterialScalars& masMaterialScalars::operator=(masMaterialScalars&& Rhs)
{
	if (this != &Rhs)
	{
		GMaterialScalars.Free(Handle);
		Handle     = Rhs.Handle;
		Rhs.Handle = {};
	}

	return *this;
}

void masMaterialScalars::SetBaseColor(const masBaseColor& BaseColor)    { MAS_MATERIAL_SET_SCALAR(BaseColor);          }
void masMaterialScalars::SetRoughness(float Roughness)                  { MAS_MATERIAL_SET_SCALAR(Roughness);          }
void masMaterialScalars::SetMetallic(float Metallic)                    { MAS_MATERIAL_SET_SCALAR(Metallic);           }
void masMaterialScalars::SetEmissiveIntensity(float EmissiveIntensity)  { MAS_MATERIAL_SET_SCALAR(EmissiveIntensity);  }
void masMaterialScalars::SetOpacity(float Opacity)                      { MAS_MATERIAL_SET_SCALAR(Opacity);            }
void masMaterialScalars::SetClearcoat(float Clearcoat)                  { MAS_MATERIAL_SET_SCALAR(Clearcoat);          }
void masMaterialScalars::SetClearcoatRoughness(float ClearcoatRoughness){ MAS_MATERIAL_SET_SCALAR(ClearcoatRoughness); }
void masMaterialScalars::SetAnisotropy(float Anisotropy)                { MAS_MATERIAL_SET_SCALAR(Anisotropy);         }
void masMaterialScalars::SetSheen(float Sheen)                          { MAS_MATERIAL_SET_SCALAR(Sheen);              }

masBaseColor masMaterialScalars::GetBaseColor()   { MAS_MATERIAL_GET_SCALAR(BaseColor,          masBaseColor()); }
float masMaterialScalars::GetRoughness()          { MAS_MATERIAL_GET_SCALAR(Roughness,          0.0f); }
float masMaterialScalars::GetMetallic()           { MAS_MATERIAL_GET_SCALAR(Metallic,           0.0f); }
float masMaterialScalars::GetEmissiveIntensity()  { MAS_MATERIAL_GET_SCALAR(EmissiveIntensity,  0.0f); }
float masMaterialScalars::GetOpacity()            { MAS_MATERIAL_GET_SCALAR(Opacity,            0.0f); }
float masMaterialScalars::GetClearcoat()          { MAS_MATERIAL_GET_SCALAR(Clearcoat,          0.0f); }
float masMaterialScalars::GetClearcoatRoughness() { MAS_MATERIAL_GET_SCALAR(ClearcoatRoughness, 0.0f); }
float masMaterialScalars::GetAnisotropy()         { MAS_MATERIAL_GET_SCALAR(Anisotropy,         0.0f); }
float masMaterialScalars::GetSheen()              { MAS_MATERIAL_GET_SCALAR(Sheen,              0.0f); }