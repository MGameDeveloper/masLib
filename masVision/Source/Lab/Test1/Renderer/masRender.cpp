#include "masRender.h"

#include "../masAssetSearch.h"
#include "Window/masWindow.h"


bool masRender::Init()
{
	Camera.Init(60.0f, masWindow_AspectRatio());
	
	Shader.LoadVertexShader(masAssetSearch("Test1", "Vertex.hlsl").Path(), "Main");
	Shader.LoadPixelShader(masAssetSearch("Test1", "Pixel.hlsl").Path(), "Main");

	return true;
}

void masRender::Terminate()
{

}

void masRender::Draw()
{

}
	
void masRender::SetCamera(const masCamera* Camera)
{
	memcpy(&this->Camera, Camera, sizeof(masCamera));
}

void masRender::SetShader(const masShader* Shader)
{
	memcpy(&this->Shader, Shader, sizeof(masCamera));
}