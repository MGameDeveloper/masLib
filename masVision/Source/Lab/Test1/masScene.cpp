#include <vector>
#include "masScene.h"

#include "Debug/Assert/masAssert.h"
#include "Graphics/masGraphics.h"
#include "masModel.h"
#include "Renderer/masRender.h"


/***********************************************************************************
*
************************************************************************************/
struct masDrawMaterial
{
	std::vector<ComPtr<ID3D11ShaderResourceView>> Textures;
	std::vector<ComPtr<ID3D11Buffer>>             ConstantBuffers;
	uint32_t ShaderID;
	uint8_t  SamplerID;
};

struct masDrawCmd
{
	ComPtr<ID3D11InputLayout>          InputLayout;
	ComPtr<ID3D11Buffer>               Vertices;
	ComPtr<ID3D11Buffer>               Indices;
	masDrawMaterial                   *Material;
	std::vector<ComPtr<ID3D11Buffer>>  ConstantBuffers;
	D3D_PRIMITIVE_TOPOLOGY             TopologyType;
	uint32_t                           IndexCount;
};

struct masDrawCmdList
{

};



/***********************************************************************************
*
************************************************************************************/
bool masScene::Init()
{
	GRender.Init();
	return true;
}

void masScene::Terminate()
{

}

void masScene::AddModel(masModel* Model)
{
	if (!Model)
		return;
	Models.push_back(std::make_unique<masEntity>(Model));
}

void masScene::AddLight(masLight* Light)
{
	if (!Light)
		return;
	Lights.push_back(std::make_unique<masEntity>(Light));
}

void masScene::Render()
{

}
