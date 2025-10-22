#include <stdio.h>

#include "masScene.h"

#include "Resources/masCamera.h"
#include "Resources/masModel.h"
#include "Resources/masLight.h"

/**********************************************************************************************************
*
***********************************************************************************************************/
#define MAS_CAMERA_POOL_SIZE 256
#define MAS_MODEL_POOL_SIZE  256
#define MAS_LIGHT_POOL_SIZE  256


/**********************************************************************************************************
*
***********************************************************************************************************/
struct masCameraPool
{
	masCamera Cameras[MAS_CAMERA_POOL_SIZE];
	uint32_t  Count;
	uint32_t  ActiveCamera;
};

struct masModelPool
{
	masModel Models[MAS_MODEL_POOL_SIZE];
	uint32_t Count;
};

struct masLightPool
{
	masLight Lights[MAS_LIGHT_POOL_SIZE];
	uint32_t Count;
};

struct masScene
{
	masCameraPool CameraPool;
	masModelPool  ModelPool;
	masLightPool  LightPool;
};

static masScene* Scene = nullptr;


/**********************************************************************************************************
*
***********************************************************************************************************/


/**********************************************************************************************************
*
***********************************************************************************************************/
bool masScene_Init()
{
	if (Scene)
		return true;

	uint64_t SceneMemSize = sizeof(masScene);
	Scene = (masScene*)malloc(SceneMemSize);
	if (!Scene)
		return false;
	memset(Scene, 0, SceneMemSize);

	// create default camera, and other objects that should be present
	masCamera* Camera = masScene_AddCamera();
	
	return true;
}

void masScene_Clean()
{
	if (!Scene)
		return;

	// loop through scene internal data and free them properly

	free(Scene);
	Scene = nullptr;
}

void masScene_Tick()
{
	// doing mouse selection, handle input movement
}

masCamera* masScene_AddCamera()
{
	// properties adjustment would be through imgui
	if (!Scene)
		return nullptr;

	static uint32_t CameraCreateCount = 0;

	masCamera* Camera = nullptr;
	for (int32_t i = 0; i < MAS_CAMERA_POOL_SIZE; ++i)
	{
		masResourceDef* Def = (masResourceDef*)&Scene->CameraPool.Cameras[i];
		if (Def->RefCount == 0)
		{
			Camera        = &Scene->CameraPool.Cameras[i];
			Def->RefCount = 1;
			sprintf_s(Def->Name, "Camera_%u", CameraCreateCount++);
			break;
		}
	}

	return Camera;
}

// void masScene_AddModel(const char* Name);  // properties adjustment would be through imgui
// void masScene_AddLight(const char* Name);  // properties adjustment would be through imgui
// void masScene_Render();