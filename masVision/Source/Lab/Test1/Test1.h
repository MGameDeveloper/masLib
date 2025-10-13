#pragma once

#include "../LabAPI.h"

struct masShader;

struct Test1 : LabAPI
{
	masShader* VertexShader;
	masShader* PixelShader;
	float      ClearColor[4];
	
	bool Init()      override;
	void Tick()      override;
	void Present()   override;
	void Terminate() override;
};