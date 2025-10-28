#include "test.h"
#include "../masPool.h"


/**************************************************************************************************
*
***************************************************************************************************/
static masPool<masMaterialData> GMaterials("Materials");


/**************************************************************************************************
*
***************************************************************************************************/

masMaterial::masMaterial(const char* Name)
{
	Handle = GMaterials.Alloc();
}

masMaterial::~masMaterial()
{
	GMaterials.Free(Handle);
}

masMaterial::masMaterial(const masMaterial& Other)
{
	Handle = Other.Handle;
	GMaterials.AddRef(Handle);
}

masMaterial::masMaterial(masMaterial&& Other)
{
	Handle = Other.Handle;
	Other.Handle = {};
}

masMaterial& masMaterial::operator=(const masMaterial& Other)
{
	if (this != &Other)
	{
		GMaterials.Free(Handle);
		Handle = Other.Handle;
		GMaterials.AddRef(Handle);
	}
	
	return *this;
}

masMaterial& masMaterial::operator=(masMaterial&& Other)
{
	if (this != &Other)
	{
		GMaterials.Free(Handle);
		Handle = Other.Handle;
		Other.Handle = {};
	}

	return *this;
}




void masMaterial::SetTexture(int32_t TextureID, const masTexture& Texture)
{

}

void masMaterial::UnSetTexture(int32_t TextureID)
{

}

void masMaterial::SetScalar(int32_t ScalarID, float Scalar)
{

}

void masMaterial::SetShader(masShader Shader)
{

}

masTexture masMaterial::GetTexture(int32_t TextureID)
{

}

masShader masMaterial::GetShader(int32_t Shader)
{

}

float masMaterial::GetScalar(int32_t ScalarID)
{

}

//masMaterial masMaterial_Create(const char* Name)
//{
//	masMaterial Mat = GMaterials.Alloc();
//	masMaterialData* Data = GMaterials.GetElement(Mat);
//}
//
//void masMaterial_Destroy(masMaterial* Material)
//{
//	GMaterials.Free(Material);
//}
//
//void masMaterial_SetShader(masMaterial Material, masShader Shader)
//{
//	masMaterialData *Data = GMaterials.GetElement(Material);
//	if (Data)
//		Data->Shader = Shader;
//}
//
//void masMaterial_SetTexture(masMaterial Material, int32_t TextureID, masTexture Texture)
//{
//	masMaterialData *Data = GMaterials.GetElement(Material);
//	if (Data)
//		Data->Textures[TextureID] = Texture;
//}