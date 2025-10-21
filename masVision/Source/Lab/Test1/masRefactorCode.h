#pragma once

#include <stdint.h>
#include <wrl/client.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


/*****************************************************************************************************
*
******************************************************************************************************/
using namespace Microsoft::WRL;
using namespace DirectX;


/*****************************************************************************************************
*
******************************************************************************************************/
#define MAS_TEXTURE_COUNT 11


/*****************************************************************************************************
*
******************************************************************************************************/
struct masResourceInfo
{
	int32_t  RefCount;
	uint32_t Type;
	char     Name[256];
};


/*****************************************************************************************************
*
******************************************************************************************************/
struct masTransform
{
	XMMATRIX World;
	XMFLOAT3 Position;
	XMFLOAT3 Rotation;
	XMFLOAT3 Scale;
};


struct masCamera
{
	XMMATRIX Projection;
	XMMATRIX View;
	XMFLOAT3 Position;
	XMFLOAT3 Rotation;
	XMFLOAT3 Target;
	float    Fov;
	float    AspectRatio;
	float    NearZ;
	float    FarZ;
};


struct masShader
{
	masResourceInfo Info;

	ID3D11InputLayout  *InputLayout;
	ID3D11VertexShader *VertexShader;
	ID3D11PixelShader  *PixelShader;
	ID3D11Buffer       *TexturesCB;
	ID3D11Buffer       *SamplersCB;
	ID3D11Buffer       *MaterialConstantsCB;
	ID3D11Buffer       *CameraCB;
	ID3D11Buffer       *TransormationCB;
};


struct masTexture
{
	masResourceInfo Info;

	ID3D11Texture2D          *pTexture2D;
	ID3D11ShaderResourceView *pSRV;
};

struct masSampler
{
	masResourceInfo Info;

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
	masResourceInfo Info;

	masTexture         Textures[MAS_TEXTURE_COUNT];
	masSampler         pSamplers[MAS_TEXTURE_COUNT];
	masMaterialScalars Scalars;
	masShader          Shader;
};


struct masMesh
{
	masResourceInfo Info;

	masTransform         Transform;
	ComPtr<ID3D11Buffer> Vertices;
	ComPtr<ID3D11Buffer> Indices;
	masMaterial          Material;
	uint32_t             IndexCount;
};

struct masModel
{
	masResourceInfo Info;

	masTransform   Transform;
	masMesh      **Meshes;
	uint32_t       MeshCount;
};


/**********************************************************************************************************
*
***********************************************************************************************************/
#define MAS_CAMERA_POOL_SIZE 5
#define MAS_MODEL_POOL_SIZE 256

struct masScene
{
	// masResourcDB
	masCamera Cameras[MAS_CAMERA_POOL_SIZE];
	masModel  Models[MAS_MODEL_POOL_SIZE];

	uint8_t ActiveCamera;
};

void masScene_Init();                      // load shaders, create samplers, resource pools
void masScene_Clean();
void masScene_Tick();                      // doing mouse selection, handle input movement
void masScene_AddCamera(const char* Name); // properties adjustment would be through imgui
void masScene_AddModel(const char* Name);  // properties adjustment would be through imgui
void masScene_Render();