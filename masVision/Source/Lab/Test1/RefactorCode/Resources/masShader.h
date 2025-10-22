#pragma once

#include <d3d11.h>

#include "masResourceDef.h"


struct masShader
{
	masResourceDef Def;

	ID3D11InputLayout*  InputLayout;
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader*  PixelShader;
	ID3D11Buffer*       TexturesCB;
	ID3D11Buffer*       SamplersCB;
	ID3D11Buffer*       MaterialConstantsCB;
	ID3D11Buffer*       CameraCB;
	ID3D11Buffer*       TransormationCB;
};