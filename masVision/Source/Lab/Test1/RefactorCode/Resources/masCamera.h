#pragma once


#include "masTransform.h"
#include "masResourceDef.h"

// fov, aspect_ratio, near_z, far_z -> recalculate projection matrix
// position, rotation -> recalculate view matrix

enum masCameraType
{
	MAS_CAMERA_TYPE_PERSPECTIVE,
	MAS_CAMERA_TYPE_ORTHOGRAPHIC,

	MAS_CAMERA_TYPE_COUNT
};

struct masCamera
{
	masResourceDef Def;

	masTransform Transform; // used to display a camera mesh for visualization in the scene and its world matrix is what is used to get view if it's used for rendering

	XMMATRIX      Projection;
	XMMATRIX      View;
	float         Fov;
	float         AspectRatio;
	float         NearZ;
	float         FarZ;
	masCameraType Type;
	bool          bProjectionDirty;
	bool          bViewDirty;
};

static void masCamera_Init(masCamera* Camera)
{
	if (!Camera)
		return;

	Camera->Transform.Position = XMFLOAT3(0.f, 0.f, -10.f);
	Camera->Transform.Rotation = XMFLOAT3(0.f, 0.f, 0.f);
	Camera->Fov                = XMConvertToRadians(60.f);
	Camera->AspectRatio        = 800.f / 600.f;
	Camera->NearZ              = 0.1f;
	Camera->FarZ               = 10000.f;
	Camera->Type               = MAS_CAMERA_TYPE_PERSPECTIVE;
	Camera->bProjectionDirty   = true;
	Camera->bViewDirty         = true;
}
static void masCamera_Tick(masCamera* Camera)
{
	if (!Camera)
		return;

	masTransform_Tick(&Camera->Transform);

	// update camera and its matrices
	if (Camera->bProjectionDirty)
	{
		if (Camera->Type == MAS_CAMERA_TYPE_PERSPECTIVE)
		{
			Camera->Projection = XMMatrixPerspectiveFovLH(Camera->Fov, Camera->AspectRatio, Camera->NearZ, Camera->FarZ);
		}
		else
		{
			float Width  = Camera->AspectRatio * Camera->Fov;
			float Height = Camera->Fov;
			Camera->Projection = XMMatrixOrthographicLH(Width, Height, Camera->NearZ, Camera->FarZ);
		}

		Camera->bProjectionDirty = false;
	}

	if (Camera->bViewDirty)
	{
		Camera->View       = XMMatrixInverse(nullptr, Camera->Transform.World);
		Camera->bViewDirty = false;
	}
}
static const XMMATRIX* masCamera_GetProjectionMatrix(masCamera* Camera)
{
	if (!Camera)
		return nullptr;
	return &Camera->Projection;
}
static const XMMATRIX* masCamera_GetViewMatrix(masCamera* Camera)
{
	if (!Camera)
		return nullptr;
	return &Camera->View;
}
static void masCamera_Position(masCamera* Camera, const XMFLOAT3& Position)
{
	if (!Camera)
		return;

	masTransform_Position(&Camera->Transform, Position);
	Camera->bViewDirty = true;
}
static void masCamera_Move(masCamera* Camera, const XMFLOAT3& Velocity)
{
	if (!Camera)
		return;

	masTransform_Move(&Camera->Transform, Velocity);
	Camera->bViewDirty = true;
}
static void masCamera_SetRotation(masCamera* Camera, const XMFLOAT3& Rotation)
{
	if (!Camera)
		return;

	masTransform_SetRotation(&Camera->Transform, Rotation);
	Camera->bViewDirty = true;
}
static void masCamera_Rotate(masCamera* Camera, const XMFLOAT3& Velocity)
{
	if (!Camera)
		return;

	masTransform_Rotate(&Camera->Transform, Velocity);
	Camera->bViewDirty = true;
}
static void masCamera_SetFov(masCamera* Camera, float Fov)
{
	if (!Camera)
		return;
	Camera->Fov = Fov;
	Camera->bProjectionDirty = true;
}
static void masCamera_SetAspectRatio(masCamera* Camera, float AspectRatio)
{
	if (!Camera)
		return;
	Camera->AspectRatio = AspectRatio;
	Camera->bProjectionDirty = true;
}
static void masCamera_SetNearZ(masCamera* Camera, float NearZ)
{
	if (!Camera)
		return;
	Camera->NearZ = NearZ;
	Camera->bProjectionDirty = true;
}
static void masCamera_SetFarZ(masCamera* Camera, float FarZ)
{
	if (!Camera)
		return;
	Camera->FarZ = FarZ;
	Camera->bProjectionDirty = true;
}
static void masCamera_SetType(masCamera* Camera, masCameraType Type)
{
	if (!Camera)
		return;
	if (Type < 0 || Type >= MAS_CAMERA_TYPE_COUNT)
		return;
	Camera->Type = Type;
	Camera->bProjectionDirty = true;
}


/*
* storing previous frame matrices
* - prev_view
* - prev_projection
* 
* standard in engines with temporal effects
*/