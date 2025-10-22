#pragma once

#include <stdint.h>


/**********************************************************************************************************
*
***********************************************************************************************************/
bool masScene_Init();                      // load shaders, create samplers, resource pools
void masScene_Clean();
void masScene_Tick();                      // doing mouse selection, handle input movement
void masScene_AddModel(const char* Name);  // properties adjustment would be through imgui
void masScene_AddLight(const char* Name);  // properties adjustment would be through imgui
void masScene_Render();


/**********************************************************************************************************
*
***********************************************************************************************************/
struct masCamera;
struct masModel;
struct masLight;

masCamera* masScene_AddCamera(); // properties adjustment would be through imgui
masCamera* masScene_ActiveCamera();
masCamera* masScene_GetCamera(uint32_t i);

masModel*  masScene_GetModel(uint32_t i);
masLight*  masScene_GetLight(uint32_t i);

uint32_t masScene_CameraCount();
uint32_t masScene_ModelCount();
uint32_t masScene_LightCount();
