#include <DirectXMath.h> // Require C++

#include "masImpl.h"

using namespace DirectX;


/***************************************************************************************************************************
*
****************************************************************************************************************************/
float mas_impl_math_fclamp(float Value, float Min, float Max)
{
	return (Value < Min) ? Min : (Value > Max) ? Max : Value;
}

float mas_impl_math_fmin(float V0, float V1)
{
	return XMMin(V0, V1);
}

float mas_impl_math_fmax(float V0, float V1)
{
	return XMMax(V0, V1);
}

float mas_impl_math_to_rad(float Degree)
{
	return XMConvertToRadians(Degree);
}

float mas_impl_math_to_deg(float Radian)
{
	return XMConvertToDegrees(Radian);
}


/***************************************************************************************************************************
* 2D Vector
****************************************************************************************************************************/
masVec2 mas_impl_math_vec2()
{
	return { 0.f, 0.f };
}
void  mas_impl_math_vec2_set(masVec2* Out, float x, float y)
{
	Out->x = x;
	Out->y = y;
}
void  mas_impl_math_vec2_add(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	Out->x = V0->x + V1->x;
	Out->y = V0->y + V1->y;
}
void  mas_impl_math_vec2_sub(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	Out->x = V0->x - V1->x;
	Out->y = V0->y - V1->y;
}
void  mas_impl_math_vec2_fmul(masVec2* Out, const masVec2* V, float Scaler)
{
	Out->x = V->x * Scaler;
	Out->y = V->y * Scaler;
}
void  mas_impl_math_vec2_mul(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	Out->x = V1->x * V1->x;
	Out->y = V1->y * V1->y;
}
void  mas_impl_math_vec2_fdiv(masVec2* Out, const masVec2* V, float Scaler)
{
	if (Scaler != 0.0f)
	{
		Out->x = V->x / Scaler;
		Out->y = V->y / Scaler;
	}
}
void  mas_impl_math_vec2_div(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	if(V1->x != 0.0f)
		Out->x = V0->x / V1->x;
	if(V1->y != 0.0f)
		Out->y = V0->y / V1->y;
}
void  mas_impl_math_vec2_negate(masVec2* Out, const masVec2* V)
{
	Out->x = -V->x;
	Out->y = -V->y;
}
void  mas_impl_math_vec2_min(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	Out->x = mas_impl_math_fmin(V0->x, V1->x);
	Out->y = mas_impl_math_fmin(V0->y, V1->y);
}
void  mas_impl_math_vec2_max(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	Out->x = mas_impl_math_fmax(V0->x, V1->x);
	Out->y = mas_impl_math_fmax(V0->y, V1->y);
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

void  mas_impl_math_vec2_perp(masVec2* Out, const masVec2* V)
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

bool mas_impl_math_vec2_near(const masVec2* V0, const masVec2* V1, const masVec2* Epsilon)
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
