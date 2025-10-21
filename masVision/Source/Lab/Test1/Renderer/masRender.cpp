#include "masRender.h"

#include "../masFileSearch.h"
#include "Window/masWindow.h"
#include "../masScene.h"
#include "../masModel.h"


/***********************************************************************************
*
************************************************************************************/
struct masDrawMaterial
{
	std::vector<ComPtr<ID3D11ShaderResourceView>> Textures;
	std::vector<ComPtr<ID3D11SamplerState>>       Samplers;
	masMaterialScalars                            Scalars;

	std::vector<ComPtr<ID3D11Buffer>>             ConstantBuffers;
	uint32_t ShaderID;
	uint8_t  SamplerID;
};

struct masDrawCmd
{
	ComPtr<ID3D11InputLayout>          InputLayout;
	ComPtr<ID3D11Buffer>               Vertices;
	ComPtr<ID3D11Buffer>               Indices;
	masDrawMaterial                    Material;
	std::vector<ComPtr<ID3D11Buffer>>  ConstantBuffers;
	D3D_PRIMITIVE_TOPOLOGY             TopologyType;
	uint32_t                           IndexCount;
	XMMATRIX                           WorldMatrix;
};

struct masDrawCmdList
{

};


/***********************************************************************************
*
************************************************************************************/
static void masBuildDrawCmdList(masDrawCmdList* CmdList, masScene* Scene)
{
	for (const auto& Model : Scene->Models)
	{
		for (const auto& Mesh : Model->Data.Model->Meshes)
		{
			masDrawCmd Cmd = { };
			Cmd.Vertices   = Mesh->pVertices;
			Cmd.Indices    = Mesh->pIndices;
			Cmd.IndexCount = Mesh->IndexCount;
			Cmd.Material; // input layout, textures, samplers, material scalar
			Cmd.WorldMatrix = Model->GetWorldMatrix();

			for (int32_t t = 0; t < MAS_TEXTURE_COUNT; ++t)
			{
				Cmd.Material.Textures.push_back(Mesh->Material->Textures[t]->pSRV);
				Cmd.Material.Samplers.push_back(Mesh->Material->Samplers[t]);
			}
			Cmd.Material.Scalars = Mesh->Material->Scalars;
			Cmd.Material.Shader  = Mesh->Material->Shader;
		}
	}
}


/***********************************************************************************
*
************************************************************************************/
void masRender::SetCamera(const masCamera& Camera)
{
	this->Camera = Camera;
}

bool masRender::Init()
{
	Camera.Init(60.0f, masWindow_AspectRatio());

	return true;
}

void masRender::Terminate()
{

}

void masRender::Draw(struct masScene* Scene)
{
	masDrawCmdList CmdList;
	masBuildDrawCmdList(&CmdList, Scene);

	int32_t i = 0;
}