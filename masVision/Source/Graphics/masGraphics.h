#pragma once

#include "masCommon.h"


bool masGraphics_Init(const void* Window, uint32_t Width, uint32_t Height);
void masGraphics_Terminate();
void masGraphics_Render();
void masGraphics_Clear(const float Color[4]);


/************************************************************************************************************
*
*************************************************************************************************************/
struct masShader;
masShader* masShader_CreateVertexShader(const char* Path, const char* Name, const char* EntryPoint);
masShader* masShader_CreatePixelShader(const char* Path, const char* Name, const char* EntryPoint);
void masShader_Destroy(masShader** Shader);