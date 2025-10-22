#pragma once

#include <DirectXMath.h>

using namespace DirectX;

enum masTransformType
{
	MAS_TRANSFORM_TYPE_LOCAL,
	MAS_TRANSFORM_TYPE_GLOBAL,

	MAS_TRANSFORM_TYPE_COUNT
};

struct masTransform
{
	const masTransform *Parent;
	XMMATRIX            World;
	XMMATRIX            Local;
	XMVECTOR            Quaternion;
	XMFLOAT3            Position;
	XMFLOAT3            Rotation;
	XMFLOAT3            Scale;
	masTransformType    Type;
	bool                bWorldRecompute;
	bool                bRotationRecompute;
};

static void masTransform_Init(masTransform* Transform)
{
	if (!Transform)
		return;

	Transform->Parent             = nullptr;
	Transform->World              = XMMatrixIdentity();
	Transform->Quaternion         = XMQuaternionIdentity();
	Transform->Position           = { 0.f, 0.f, 0.f };
	Transform->Rotation           = { 0.f, 0.f, 0.f };
	Transform->Scale              = { 1.f, 1.f, 1.f };
	Transform->bWorldRecompute    = true;
	Transform->bRotationRecompute = true;
}
static void masTransform_Tick(masTransform* Transform)
{
	if (!Transform)
		return;

	if (Transform->bRotationRecompute)
	{
		XMVECTOR Angles = XMVectorSet(
			XMConvertToRadians(Transform->Rotation.x),
			XMConvertToRadians(Transform->Rotation.y),
			XMConvertToRadians(Transform->Rotation.z),
			0.f);
		Transform->Quaternion = XMQuaternionRotationRollPitchYawFromVector(Angles);
		Transform->bRotationRecompute = false;
	}

	if (Transform->bWorldRecompute)
	{
		XMVECTOR Position = XMLoadFloat3(&Transform->Position);
		XMVECTOR Scale    = XMLoadFloat3(&Transform->Scale);
		Transform->Local  = (XMMatrixScalingFromVector(Scale) * XMMatrixRotationQuaternion(Transform->Quaternion) * XMMatrixTranslationFromVector(Position));
		if (Transform->Parent && Transform->Type == MAS_TRANSFORM_TYPE_LOCAL)
			Transform->World = Transform->Local * Transform->Parent->World;
		else
			Transform->World = Transform->Local;

		Transform->bWorldRecompute = false;
	}
}
static void masTransform_SetParent(masTransform* Transform, const masTransform* Parent)
{
	if (!Transform || !Parent)
		return;
	Transform->Parent = Parent;
	Transform->bWorldRecompute = true;
}
static void masTransform_Position(masTransform* Transform, const XMFLOAT3& Position)
{
	if (!Transform)
		return;

	Transform->Position = Position;
	Transform->bWorldRecompute = true;
}
static void masTransform_Move(masTransform* Transform, const XMFLOAT3& Velocity)
{
	if (!Transform)
		return;

	XMStoreFloat3(&Transform->Position,
		XMVectorAdd(XMLoadFloat3(&Transform->Position), XMLoadFloat3(&Velocity)));
	Transform->bWorldRecompute = true;
}
static void masTransform_SetRotation(masTransform* Transform, const XMFLOAT3& Rotation)
{
	if (!Transform)
		return;

	Transform->Rotation           = Rotation;
	Transform->bRotationRecompute = true;
	Transform->bWorldRecompute    = true;
}
static void masTransform_Rotate(masTransform* Transform, const XMFLOAT3& Velocity)
{
	if (!Transform)
		return;

	XMStoreFloat3(&Transform->Rotation,
		XMVectorAdd(XMLoadFloat3(&Transform->Rotation), XMLoadFloat3(&Velocity)));
	Transform->bRotationRecompute = true;
	Transform->bWorldRecompute    = true;
}
static void masTransform_SetScale(masTransform* Transform, const XMFLOAT3& Scale)
{
	if (!Transform)
		return;

	Transform->Scale = Scale;
	Transform->bWorldRecompute = true;
}
static void masTransform_Scale(masTransform* Transform, const XMFLOAT3& Velocity)
{
	XMStoreFloat3(&Transform->Scale,
		XMVectorAdd(XMLoadFloat3(&Transform->Scale), XMLoadFloat3(&Velocity)));
	Transform->bWorldRecompute = true;
}
static void masTransform_SetType(masTransform* Transform, masTransformType Type)
{
	if (!Transform)
		return;
	Transform->Type            = Type;
	Transform->bWorldRecompute = true;
}
static const XMMATRIX* masTransform_GetMatrix(masTransform* Transform)
{
	if (!Transform)
		return nullptr;

	return (Transform->Type == MAS_TRANSFORM_TYPE_LOCAL) ? &Transform->Local : &Transform->World;
}

// 2. Add Quaternion Setter (Optional) -> be aware that in tick we generate quaternion from euler setting quaternion directly is buggy
// 4. Add Directional Accessors(Optional)
// 5. Optional: Dirty Flag Propagation -> inform all children to be updated to correctly follow their parent
