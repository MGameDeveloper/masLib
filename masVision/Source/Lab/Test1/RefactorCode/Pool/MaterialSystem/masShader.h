#pragma once

#include "../masHandle.h"

class masShader
{
private:
	masHandle VertexShader;
	masHandle PixelShader;
	masHandle InputLayout;

public:
	masShader();
	~masShader();
	masShader(const masShader& Other)          = delete;
	masShader(masShader&& Other)               = delete;
	masShader& operator=(const masShader& Rhs) = delete;
	masShader& operator=(masShader&& Rhs)      = delete;
};