#include <vector>

#include "Test1.h"
#include "Graphics/masGraphics.h"
#include "Time/masTime.h"
#include "masModel.h"


/*****************************************************************************************************
*
******************************************************************************************************/
const masD3D11 *dx    = nullptr;
float           ClearColor[4];
std::vector<masModel*> Models;


/*****************************************************************************************************
*
******************************************************************************************************/
bool Test1::Init()
{
	dx = masGraphics_D3D11();
	memset(ClearColor, 0, sizeof(float) * 4);

	Models.push_back(masModel_Load("RotaryCannon"));
	Models.push_back(masModel_Load("AK47"));
	
	return true;
}

void Test1::Tick()
{
	ClearColor[0] = sinf((float)masTime_Now());
}

void Test1::Present()
{
	dx->ImmediateContext->OMSetRenderTargets(1, dx->RenderTargetView.GetAddressOf(), nullptr);
	dx->ImmediateContext->ClearRenderTargetView(dx->RenderTargetView.Get(), ClearColor);

	for (int32_t i = 0; i < Models.size(); ++i)
		masModel_Draw(Models[i]);

	dx->SwapChain->Present(0, 0);
}

void Test1::Terminate()
{
	for (int32_t i = 0; i < Models.size(); ++i)
		masModel_UnLoad(&Models[i]);
}
