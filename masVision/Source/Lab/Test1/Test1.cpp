#include "Test1.h"
#include "Graphics/masGraphics.h"
#include "Time/masTime.h"

#include "masModel.h"


/*****************************************************************************************************
*
******************************************************************************************************/
const masD3D11 *dx    = nullptr;
masModel       *Model = nullptr;
float           ClearColor[4];


/*****************************************************************************************************
*
******************************************************************************************************/
bool Test1::Init()
{
	dx = masGraphics_D3D11();
	memset(ClearColor, 0, sizeof(float) * 4);

	Model = masModel_Load("Assets/Models/AK47/scene.gltf");

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
	dx->SwapChain->Present(0, 0);
}

void Test1::Terminate()
{
	masModel_UnLoad(&Model);
}
