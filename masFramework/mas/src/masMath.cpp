#include <DirectXMath.h> // Require C++

#include "masTypes.h"

using namespace DirectX;

extern "C"
{

/***************************************************************************************************************************
*
****************************************************************************************************************************/
float mas_impl_math_float_clamp(float Value, float Min, float Max)
{
	return (Value < Min) ? Min : (Value > Max) ? Max : Value;
}
float mas_impl_math_float_min(float V0, float V1)
{
	return XMMin(V0, V1);
}
float mas_impl_math_float_max(float V0, float V1)
{
	return XMMax(V0, V1);
}
float mas_impl_math_deg_to_rad(float Degree)
{
	return XMConvertToRadians(Degree);
}
float mas_impl_math_rad_to_deg(float Radian)
{
	return XMConvertToDegrees(Radian);
}


/***************************************************************************************************************************
* 2D Vector
****************************************************************************************************************************/
void  mas_impl_math_vec2_set(masVec2* Out, float x, float y)
{
	Out->x = x;
	Out->y = y;
}
void  mas_impl_math_vec2_add(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	XMVECTOR Vec0   = XMVectorSet(V0->x, V0->y, 0.0f, 0.0f);
	XMVECTOR Vec1   = XMVectorSet(V1->x, V1->y, 0.0f, 0.0f);
	XMVECTOR Result = XMVectorAdd(Vec0, Vec1);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
void  mas_impl_math_vec2_sub(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	XMVECTOR Vec0   = XMVectorSet(V0->x, V0->y, 0.0f, 0.0f);
	XMVECTOR Vec1   = XMVectorSet(V1->x, V1->y, 0.0f, 0.0f);
	XMVECTOR Result = XMVectorSubtract(Vec0, Vec1);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
void  mas_impl_math_vec2_mul_scale(masVec2* Out, const masVec2* V, float Scaler)
{
	XMVECTOR Vec    = XMVectorSet(V->x, V->y, 0.0f, 0.0f);
	XMVECTOR Result = XMVectorScale(Vec, Scaler);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
void  mas_impl_math_vec2_mul(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	XMVECTOR Vec0   = XMVectorSet(V0->x, V0->y, 0.0f, 0.0f);
	XMVECTOR Vec1   = XMVectorSet(V1->x, V1->y, 0.0f, 0.0f);
	XMVECTOR Result = XMVectorMultiply(Vec0, Vec1);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
void  mas_impl_math_vec2_div_scale(masVec2* Out, const masVec2* V, float Scaler)
{
	XMVECTOR Vec       = XMVectorSet(V->x, V->y, 0.0f, 0.0f);
	XMVECTOR VecScaler = XMVectorSet(Scaler, Scaler, 0.0f, 0.0f);
	XMVECTOR Result    = XMVectorDivide(Vec, VecScaler);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
void  mas_impl_math_vec2_div(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	XMVECTOR Vec0   = XMVectorSet(V0->x, V0->y, 0.0f, 0.0f);
	XMVECTOR Vec1   = XMVectorSet(V1->x, V1->y, 0.0f, 0.0f);
	XMVECTOR Result = XMVectorDivide(Vec0, Vec1);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
void  mas_impl_math_vec2_negate(masVec2* Out, const masVec2* V)
{
	XMVECTOR Vec    = XMVectorSet(V->x, V->y, 0.0f, 0.0f);
	XMVECTOR Result = XMVectorNegate(Vec);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Vec);
}
void  mas_impl_math_vec2_min(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	XMVECTOR Vec0   = XMVectorSet(V0->x, V0->y, 0.0f, 0.0f);
	XMVECTOR Vec1   = XMVectorSet(V1->x, V1->y, 0.0f, 0.0f);
	XMVECTOR Result = XMVectorMin(Vec0, Vec1);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
void  mas_impl_math_vec2_max(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	XMVECTOR Vec0   = XMVectorSet(V0->x, V0->y, 0.0f, 0.0f);
	XMVECTOR Vec1   = XMVectorSet(V1->x, V1->y, 0.0f, 0.0f);
	XMVECTOR Result = XMVectorMax(Vec0, Vec1);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
float mas_impl_math_vec2_distance(const masVec2* V0, const masVec2* V1)
{
	masVec2 Diff;
	mas_impl_math_vec2_sub(&Diff, V0, V1);

	XMVECTOR Vec    = XMVectorSet(Diff.x, Diff.y, 0.f, 0.f);
	XMVECTOR Result = XMVector2Length(Vec);
	return XMVectorGetX(Result);
}
float mas_impl_math_vec2_mag(const masVec2* V)
{
	XMVECTOR Vec    = XMVectorSet(V->x, V->y, 0.f, 0.f);
	XMVECTOR Result = XMVector2Length(Vec);
	return XMVectorGetX(Result);
}
void  mas_impl_math_vec2_perpendicular(masVec2* Out, const masVec2* V)
{
	XMVECTOR Vec = XMVectorSet(V->x, V->y, 0.f, 0.f);
	XMVECTOR Result = XMVector2Orthogonal(Vec);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
void  mas_impl_math_vec2_norm(masVec2* Out, const masVec2* V)
{
	XMVECTOR Vec    = XMVectorSet(V->x, V->y, 0.f, 0.f);
	XMVECTOR Result = XMVector2Normalize(Vec);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
float mas_impl_math_vec2_dot(const masVec2* V0, const masVec2* V1)
{
	XMVECTOR Vec0   = XMVectorSet(V0->x, V0->y, 0.f, 0.f);
	XMVECTOR Vec1   = XMVectorSet(V1->x, V1->y, 0.f, 0.f);
	XMVECTOR Result = XMVector2Dot(Vec0, Vec1);
	return XMVectorGetX(Result);
}
void  mas_impl_math_vec2_lerp(masVec2* Out, const masVec2* V0, const masVec2* V1, float T)
{
	XMVECTOR Vec0   = XMVectorSet(V0->x, V0->y, 0.f, 0.f);
	XMVECTOR Vec1   = XMVectorSet(V1->x, V1->y, 0.f, 0.f);
	XMVECTOR Result = XMVectorLerp(Vec0, Vec1, T);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
void  mas_impl_math_vec2_clamp(masVec2* Out, const masVec2* V, float Min, float Max)
{
	XMVECTOR Vec    = XMVectorSet(V->x, V->y, 0.f, 0.f);
	XMVECTOR Result = XMVector2ClampLength(Vec, Min, Max);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
bool  mas_impl_math_vec2_equals(const masVec2* V0, const masVec2* V1)
{
	XMVECTOR Vec0 = XMVectorSet(V0->x, V0->y, 0.f, 0.f);
	XMVECTOR Vec1 = XMVectorSet(V1->x, V1->y, 0.f, 0.f);
	return XMVector2Equal(Vec0, Vec1);
}
bool  mas_impl_math_vec2_near(const masVec2* V0, const masVec2* V1, const masVec2* Epsilon)
{
	XMVECTOR Vec0       = XMVectorSet(V0->x, V0->y, 0.f, 0.f);
	XMVECTOR Vec1       = XMVectorSet(V1->x, V1->y, 0.f, 0.f);
	XMVECTOR VecEpsilon = XMVectorSet(Epsilon->x, Epsilon->y, 0.0001f, 0.0001f);
	return XMVector2NearEqual(Vec0, Vec1, VecEpsilon);
}
void  mas_impl_math_vec2_reflect(masVec2* Out, const masVec2* V, const masVec2* Normal)
{
	XMVECTOR Vec    = XMVectorSet(V->x, V->y, 0.f, 0.f);
	XMVECTOR Norm   = XMVectorSet(Normal->x, Normal->y, 0.f, 0.f);
	XMVECTOR Result = XMVector2Reflect(Vec, Norm);
	XMStoreFloat2((XMFLOAT2*)Out->xy, Result);
}
float mas_impl_math_vec2_angle(const masVec2* V0, const masVec2* V1)
{
	XMVECTOR Vec0  = XMVectorSet(V0->x, V0->y, 0.f, 0.f);
	XMVECTOR Vec1  = XMVectorSet(V1->x, V1->y, 0.f, 0.f);
	XMVECTOR Angle = XMVector2AngleBetweenVectors(Vec0, Vec1);
	return XMVectorGetX(Angle);
}


/***************************************************************************************************************************
* 3D Vector
****************************************************************************************************************************/
void  mas_impl_vec3_set(masVec3* Out, float x, float y, float z)
{
	if (Out)
	{
		Out->x = x;
		Out->y = y;
		Out->z = z;
	}
}
void  mas_impl_vec3_add(masVec3* Out, const masVec3* V0, const masVec3* V1)
{
	if (Out && V0 && V1)
	{
		XMVECTOR Vec0 = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1 = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVectorAdd(Vec0, Vec1);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_sub(masVec3* Out, const masVec3* V0, const masVec3* V1)
{
	if (Out && V0 && V1)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1   = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVectorSubtract(Vec0, Vec1);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_mul_scale(masVec3* Out, const masVec3* V, float Scaler)
{
	if (Out && V)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V);
		XMVECTOR Result = XMVectorScale(Vec0, Scaler);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_mul(masVec3* Out, const masVec3* V0, const masVec3* V1)
{
	if (Out && V0 && V1)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1   = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVectorMultiply(Vec0, Vec1);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_div_scale(masVec3* Out, const masVec3* V, float Scaler)
{
	if (Out && V)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V);
		XMVECTOR Vec1   = XMVectorSet(Scaler, Scaler, Scaler, 0.f);
		XMVECTOR Result = XMVectorDivide(Vec0, Vec1);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_div(masVec3* Out, const masVec3* V0, const masVec3* V1)
{
	if (Out && V0 && V1)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1   = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVectorDivide(Vec0, Vec1);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_negate(masVec3* Out)
{
	if (Out)
	{
		XMVECTOR Vec    = XMLoadFloat3((XMFLOAT3*)Out);
		XMVECTOR Result = XMVectorNegate(Vec);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
float mas_impl_vec3_length(const masVec3* V)
{
	if (V)
	{
		XMVECTOR Vec    = XMLoadFloat3((XMFLOAT3*)V);
		XMVECTOR Result = XMVector3Length(Vec);
		return XMVectorGetX(Result);
	}

	return 0.0f;
}
float mas_impl_vec3_length_sq(const masVec3* V)
{
	if (V)
	{
		XMVECTOR Vec    = XMLoadFloat3((XMFLOAT3*)V);
		XMVECTOR Result = XMVector3LengthSq(Vec);
		return XMVectorGetX(Result);
	}

	return 0.0f;
}
void  mas_impl_vec3_norm(masVec3* Out, const masVec3* V)
{
	if (Out && V)
	{
		XMVECTOR Vec    = XMLoadFloat3((XMFLOAT3*)V);
		XMVECTOR Result = XMVector3Normalize(Vec);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
float mas_impl_vec3_dot(const masVec3* V0, const masVec3* V1)
{
	if (V0 && V1)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1   = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVector3Dot(Vec0, Vec1);
		return XMVectorGetX(Result);
	}

	return 0.0f;
}
void  mas_impl_vec3_cross(masVec3* Out, const masVec3* V0, const masVec3* V1)
{
	if (Out && V0 && V1)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1   = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVector3Cross(Vec0, Vec1);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_min(masVec3* Out, const masVec3* V0, const masVec3* V1)
{
	if (Out && V0 && V1)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1   = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVectorMin(Vec0, Vec1);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_max(masVec3* Out, const masVec3* V0, const masVec3* V1)
{
	if (Out && V0 && V1)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1   = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVectorMax(Vec0, Vec1);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_lerp(masVec3* Out, const masVec3* V0, const masVec3* V1, float T)
{
	if (Out && V0)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1   = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVectorLerp(Vec0, Vec1, T);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
float mas_impl_vec3_angle(const masVec2* V0, const masVec2* V1)
{
	if (V0 && V1)
	{
		XMVECTOR Vec0   = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1   = XMLoadFloat3((XMFLOAT3*)V1);
		XMVECTOR Result = XMVector3AngleBetweenVectors(Vec0, Vec1);
		return XMVectorGetX(Result);
	}
}
void  mas_impl_vec3_reflect(masVec3* Out, const masVec3* V, const masVec3* Normal)
{
	if (Out && V && Normal)
	{
		XMVECTOR Vec       = XMLoadFloat3((XMFLOAT3*)V);
		XMVECTOR NormalVec = XMLoadFloat3((XMFLOAT3*)Normal);
		XMVECTOR Result    = XMVector3Reflect(Vec, NormalVec);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_rotate_quaternion(masVec3* Out, const masVec3* V, const masQuaternion* Q)
{
	if (Out && V && Q)
	{
		XMVECTOR Vec     = XMLoadFloat3((XMFLOAT3*)V);
		XMVECTOR QuatVec = XMLoadFloat4((XMFLOAT4*)Q);
		XMVECTOR Result  = XMVector3Rotate(Vec, QuatVec);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_perpendicular(masVec3* Out, const masVec3* V)
{
	if (V)
	{
		XMVECTOR Vec    = XMLoadFloat3((XMFLOAT3*)V);
		XMVECTOR Result = XMVector3Orthogonal(Vec);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
void  mas_impl_vec3_clamp(masVec3* Out, const masVec3* V, float Min, float Max)
{
	if (Out && V)
	{
		XMVECTOR Vec    = XMLoadFloat3((XMFLOAT3*)V);
		XMVECTOR Result = XMVector3ClampLength(Vec, Min, Max);
		XMStoreFloat3((XMFLOAT3*)Out, Result);
	}
}
bool  mas_impl_vec3_equals(const masVec3* V0, const masVec3* V1)
{
	if (V0 && V1)
	{
		XMVECTOR Vec0 = XMLoadFloat3((XMFLOAT3*)V0);
		XMVECTOR Vec1 = XMLoadFloat3((XMFLOAT3*)V1);
		return XMVector3Equal(Vec0, Vec1);
	}

	return false;
}
float mas_impl_vec3_distance(const masVec3* V0, const masVec3* V1);
float mas_impl_vec3_distance_sq(const masVec3* V0, const masVec3* V1);
void  mas_impl_vec3_direction(masVec3* Out, const masVec3* V0, const masVec3* V1);


/***************************************************************************************************************************
* 4D Vector
****************************************************************************************************************************/


/***************************************************************************************************************************
* Quaternion
****************************************************************************************************************************/
void mas_impl_math_quaternion_init(masQuaternion* Out)
{
	if (Out)
	{
		*Out = { 0.0 };
		Out->w = 1.f;
	}
}

void mas_impl_math_quaternion_from_euler(masQuaternion* Out, const masVec3* Euler)
{
	if (Out && Euler)
	{
		XMVECTOR EulerVec = XMLoadFloat3((XMFLOAT3*)Euler);
		XMVECTOR Quat     = XMQuaternionRotationRollPitchYawFromVector(EulerVec);
		XMStoreFloat4((XMFLOAT4*)Out, Quat);
	}
}


/***************************************************************************************************************************
* Matrix
****************************************************************************************************************************/
void mas_impl_math_matrix_init(masMatrix* Out)
{
	if (Out)
	{
		XMMATRIX Identity = XMMatrixIdentity();
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Identity);
	}
}

void mas_impl_math_matrix_multiply(masMatrix* Out, const masMatrix* M0, const masMatrix* M1)
{
	if (Out && M0 && M1)
	{
		XMMATRIX Mat0   = XMLoadFloat4x4((XMFLOAT4X4*)M0);
		XMMATRIX Mat1   = XMLoadFloat4x4((XMFLOAT4X4*)M1);
		XMMATRIX Result = XMMatrixMultiply(Mat0, Mat1);
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Result);
	}
}

void mas_impl_math_matrix_transpose(masMatrix* Out, const masMatrix* M)
{
	if (Out && M)
	{
		XMMATRIX Mat    = XMLoadFloat4x4((XMFLOAT4X4*)M);
		XMMATRIX Result = XMMatrixTranspose(Mat);
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Result);
	}
}

void mas_impl_math_matrix_inverse(masMatrix* Out, const masMatrix* M)
{
	if (Out && M)
	{
		XMMATRIX Mat = XMLoadFloat4x4((XMFLOAT4X4*)M);
		XMVECTOR Det = XMMatrixDeterminant(Mat);
		XMMATRIX Result = XMMatrixInverse(&Det, Mat);
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Result);
	}
}

void mas_impl_math_matrix_translate(masMatrix* Out, const masMatrix* M, const masVec3* V)
{
	if (Out && M && V)
	{
		XMMATRIX Mat    = XMLoadFloat4x4((XMFLOAT4X4*)M);
		XMMATRIX Result = Mat * XMMatrixTranslation(V->x, V->y, V->z);
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Result);
	}
}

void mas_impl_math_matrix_rotate_euler(masMatrix* Out, const masMatrix* M, const masVec3* Euler)
{
	if (Out && M && Euler)
	{
		XMMATRIX Mat       = XMLoadFloat4x4((XMFLOAT4X4*)M);
		XMMATRIX RotateMat = XMMatrixRotationX(Euler->x);
		RotateMat         *= XMMatrixRotationY(Euler->y);
		RotateMat         *= XMMatrixRotationZ(Euler->z);
		XMMATRIX Result = Mat * RotateMat;
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Result);
	}
}

void mas_impl_math_matrix_rotate_quaternion(masMatrix* Out, const masMatrix *M, const masQuaternion* Q)
{
	if (Out && M && Q)
	{
		XMVECTOR Quat   = XMLoadFloat4((XMFLOAT4*)Q);
		XMMATRIX Mat    = XMLoadFloat4x4((XMFLOAT4X4*)M);
		XMMATRIX Result = XMMatrixRotationQuaternion(Quat);
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Mat);
	}
}

void mas_impl_math_matrix_scale(masMatrix* Out, const masMatrix* M, const masVec3* V)
{
	if (Out && M && V)
	{
		XMMATRIX Mat    = XMLoadFloat4x4((XMFLOAT4X4*)M);
		XMMATRIX Result = Mat * XMMatrixScaling(V->x, V->y, V->z);
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Result);
	}
}

void mas_impl_math_matrix_transform(masMatrix* Out, const masVec3* Translate, const masVec3* Euler, const masVec3* Scale);

void mas_impl_math_matrix_perspective(masMatrix* Out, float Fov, float AspectRatio, float NearZ, float FarZ)
{
	if (Out)
	{
		XMMATRIX Mat = XMMatrixPerspectiveFovLH(Fov, AspectRatio, NearZ, FarZ);
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Mat);
	}
}

void mas_impl_math_matrix_orthographic(masMatrix* Out, float ViewWidth, float ViewHeight, float NearZ, float FarZ)
{
	if (Out)
	{
		XMMATRIX Mat = XMMatrixOrthographicLH(ViewWidth, ViewHeight, NearZ, FarZ);
		XMStoreFloat4x4((XMFLOAT4X4*)Out, Mat);
	}
}

void mas_impl_math_matrix_view();

void mat_impl_math_matrix_decompose(const masMatrix* M, masVec3* OutScale, masQuaternion* OutQuat, masVec3* OutTranslate)
{
	if (M)
	{
		XMVECTOR Scale, Quat, Translate;
		XMMATRIX Mat = XMLoadFloat4x4((XMFLOAT4X4*)M);
		XMMatrixDecompose(&Scale, &Quat, &Translate, Mat);

		if (OutScale)
			XMStoreFloat3((XMFLOAT3*)OutScale, Scale);

		if (OutQuat)
			XMStoreFloat4((XMFLOAT4*)OutQuat, Quat);

		if (OutTranslate)
			XMStoreFloat3((XMFLOAT3*)OutTranslate, Translate);
	}

}

}
