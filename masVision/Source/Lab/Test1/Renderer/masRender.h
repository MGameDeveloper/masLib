#pragma once

#include "masCamera.h"
#include "masShader.h"

struct masRender
{
	masCamera Camera;

	masRender()  {};
	~masRender() {};

	void SetCamera(const masCamera& Camera);

	bool Init();
	void Terminate();
	void Draw(struct masScene* Scene);
};