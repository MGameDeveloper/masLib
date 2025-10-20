#pragma once

#include "masCamera.h"
#include "masShader.h"

struct masRender
{
	masCamera Camera;
	masShader Shader;

	masRender()  {};
	~masRender() {};

	bool Init();
	void Terminate();
	void Draw();

	void SetCamera(const masCamera* Camera);
	void SetShader(const masShader* Shader);
};