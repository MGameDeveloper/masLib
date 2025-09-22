#include <DirectXMath.h> // Require C++

#include "masImpl.h"

using namespace DirectX;

float mas_impl_math_to_rad(float degree)
{
	static float f = XM_PI / 180.f;
	return degree * f;
}

float mas_impl_math_to_deg(float radian)
{
	static float f = 180.f / XM_PI;
	return radian * f;
}

masVec2 mas_impl_vec2()
{
	return { 0.f, 0.f };
}

void mas_impl_vec2_set(masVec2* Out, float x, float y)
{
	Out->x = x;
	Out->y = y;
}

void mas_impl_vec2_add(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	Out->x = V0->x + V1->x;
	Out->y = V0->y + V1->y;
}

void mas_impl_vec2_sub(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	Out->x = V0->x - V1->x;
	Out->y = V0->y - V1->y;
}

void mas_impl_vec2_mul(masVec2* Out, const masVec2* V, float Scaler)
{
	Out->x = V->x * Scaler;
	Out->y = V->y * Scaler;
}

void mas_impl_vec2_mul(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	Out->x = V1->x * V1->x;
	Out->y = V1->y * V1->y;
}

void mas_impl_vec2_div(masVec2* Out, const masVec2* V, float Scaler)
{
	if (Scaler != 0.0f)
	{
		Out->x = V->x / Scaler;
		Out->y = V->y / Scaler;
	}
}

void mas_impl_vec2_div(masVec2* Out, const masVec2* V0, const masVec2* V1)
{
	if(V1->x != 0.0f)
		Out->x = V0->x / V1->x;
	if(V1->y != 0.0f)
		Out->y = V0->y / V1->y;
}

// Geometric Operations
float mas_impl_vec2_mag(const masVec2* V)
{
	return sqrtf(V->x * V->x + V->y * V->y);
}

void  mas_impl_vec2_norm(masVec2* Out, const masVec2* V)
{
	float mag = mas_impl_vec2_mag(V);
	if (mag == 0.0f)
		return;
	Out->x = V->x / mag;
	Out->y = V->y / mag;
}

float mas_impl_vec2_dot(const masVec2* V0, const masVec2* V1)
{
	return (V0->x * V1->x + V0->y * V1->y);
}

float mas_impl_vec2_angle(const masVec2* V0, const masVec2* V1)
{

}

float mas_impl_vec2_distance(const masVec2* V0, const masVec2* V1)
{
	
}

// Directional & Transform
void  mas_impl_vec2_project(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_impl_vec2_negate(masVec2* Out, const masVec2* V);
void  mas_impl_vec2_perp(masVec2* Out, const masVec2* V);
void  mas_impl_vec2_lerp(masVec2* Out, const masVec2* V0, const masVec2* V1, float T);
void  mas_impl_vec2_reflect(masVec2* Out, const masVec2* V, const masVec2* Normal);
void  mas_impl_vec2_clamp(masVec2* Out, float Min, float Max, const masVec2* V);
bool  mas_impl_vec2_equals(const masVec2* V0, const masVec2* V1);
void  mas_impl_vec2_min(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_impl_vec2_max(masVec2* Out, const masVec2* V0, const masVec2* V1);
bool  mas_impl_vec2_near(const masVec2* V0, const masVec2* V1, float Epsilon);
void  mas_impl_vec2_rotate(masVec2* Out, const masVec2* V, float radius);