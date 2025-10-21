#include <vector>
#include "masScene.h"

#include "Debug/Assert/masAssert.h"
#include "Graphics/masGraphics.h"
#include "masModel.h"
#include "Renderer/masRender.h"


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
	GRender.Draw(this);
}
