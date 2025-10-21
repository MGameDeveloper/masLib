#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct masCamera
{
	XMMATRIX Projection;
	XMMATRIX View;

	XMFLOAT3 Rot;
	XMFLOAT3 Pos;
	XMFLOAT3 Target;
	float    Fov;
	float    NearZ;
	float    FarZ;
	float    AspectRatio;
	bool     bRecalculate;

	masCamera() :
		Fov(45), AspectRatio(1.5), NearZ(0.1f), FarZ(10000.f),
		Pos(0.f, 0.f, -10.f), Rot(0.f, 0.f, 0.f), Projection(), View(),
		bRecalculate(true), Target(0.f, 0.f, 0.f)
	{
		Init(Fov, AspectRatio);
	}

	~masCamera() {};

	masCamera(float InFov, float InAspectRatio, float InNearZ = 0.1f, float InFarZ = 10000.f) :
		Fov(InFov), AspectRatio(InAspectRatio), NearZ(InNearZ), FarZ(InFarZ), 
		Pos(0.f, 0.f, -10.f), Rot(0.f, 0.f, 0.f), Projection(), View(),
		bRecalculate(true), Target(0.f, 0.f, 0.f)
	{
		Init(InFov, InAspectRatio);
	}

	void Init(float InFov_Degree, float InAspectRatio, float InNearZ = 0.1f, float InFarZ = 10000.f)
	{
		Fov          = XMConvertToRadians(InFov_Degree);
		AspectRatio  = InAspectRatio;
		NearZ        = InNearZ;
		FarZ         = InFarZ;
		Pos          = { 0.f, 0.f, -10.f };
		Rot          = { 0.f, 0.f, 0.f };
		Target       = { 0.f, 0.f, 0.f };
		bRecalculate = true;

		Projection = XMMatrixPerspectiveFovLH(Fov, AspectRatio, NearZ, FarZ);

		XMVECTOR LookDirction = XMVectorSubtract(XMLoadFloat3(&Target), XMLoadFloat3(&Pos));
		View = XMMatrixLookAtLH(XMLoadFloat3(&Pos), LookDirction, XMVectorSet(0.f, 1.f, 0.f, 0.f));

		bRecalculate = false;
	}

	XMMATRIX GetViewMatrix()
	{
		return XMMatrixTranspose(View);
	}

	XMMATRIX GetProjectionMatrix()
	{
		return XMMatrixTranspose(Projection);
	}
};