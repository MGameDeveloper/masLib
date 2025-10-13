#include "Test1.h"

#include <stdio.h>
#include "Graphics/masGraphics.h"
#include "Time/masTime.h"


bool Test1::Init()
{
	memset(ClearColor, 0, sizeof(float) * 4);

	VertexShader = masShader_CreateVertexShader("Shaders/Default.vs", "DefaultShader_VS", "masMain");
	PixelShader  = masShader_CreatePixelShader("Shaders/Default.ps", "DefaultShader_PS", "masMain");
	
	return true;
}

void Test1::Tick()
{
	ClearColor[0] = sin(masTime_Now());
}

void Test1::Present()
{
	masGraphics_Clear(ClearColor);

	masGraphics_Render();
}

void Test1::Terminate()
{
	masShader_Destroy(&VertexShader);
	masShader_Destroy(&PixelShader);
}
