#include "masRender.h"

#include "../masFileSearch.h"
#include "Window/masWindow.h"


bool masRender::Init()
{
	Camera.Init(60.0f, masWindow_AspectRatio());
	
	Shader.LoadVertexShader(masFileSearch("Test1", "Vertex.hlsl").Path(), "Main");
	Shader.LoadPixelShader(masFileSearch("Test1", "Pixel.hlsl").Path(), "Main");

	return true;
}

void masRender::Terminate()
{

}

void masRender::Draw()
{

}
	
void masRender::SetCamera(const masCamera& Camera)
{
	this->Camera = Camera;
}

void masRender::SetShader(const masShader& Shader)
{
	this->Shader = Shader;
}