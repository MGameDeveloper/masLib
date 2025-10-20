#include <vector>

#include "Test1.h"
#include "Graphics/masGraphics.h"
#include "Time/masTime.h"
#include "masModel.h"
#include "masScene.h"


/*****************************************************************************************************
*
******************************************************************************************************/
masScene        Scene;


/*****************************************************************************************************
*
******************************************************************************************************/
bool Test1::Init()
{
	Scene.Init();
	Scene.AddModel(masModel_Load("RotaryCannon"));
	Scene.AddModel(masModel_Load("AK47"));

	return true;
}

void Test1::Tick()
{
	
}

void Test1::Present()
{
	masGraphicsUI_Prepare();

	Scene.Render();

	masGraphicsUI_Render();

	masGraphics_Present();
}

void Test1::Terminate()
{
	Scene.Terminate();
}
