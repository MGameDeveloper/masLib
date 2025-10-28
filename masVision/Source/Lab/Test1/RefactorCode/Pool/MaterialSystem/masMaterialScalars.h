#pragma once

#include "../masHandle.h"

struct masBaseColor
{
	float r, g, b, a;
	masBaseColor():
		r(0.f), g(0.f), b(0.f), a(0.f)
	{}
};

class masMaterialScalars
{
private:
	masHandle Handle;

public:
	masMaterialScalars();
	~masMaterialScalars();
	masMaterialScalars(const masMaterialScalars& Other);
	masMaterialScalars(masMaterialScalars&& Other);
	masMaterialScalars& operator=(const masMaterialScalars& Rhs);
	masMaterialScalars& operator=(masMaterialScalars&& Rhs);

	void SetBaseColor(const masBaseColor& BaseColor);
	void SetRoughness(float Roughness);
	void SetMetallic(float Metallic);
	void SetEmissiveIntensity(float EmissiveIntensity);
	void SetOpacity(float Opacity);
	void SetClearcoat(float Clearcoat);
	void SetClearcoatRoughness(float ClearcoatRoughness);
	void SetAnisotropy(float Anisotropy);
	void SetSheen(float Sheen);

	masBaseColor GetBaseColor();
	float GetRoughness();
	float GetMetallic();
	float GetEmissiveIntensity();
	float GetOpacity();
	float GetClearcoat();
	float GetClearcoatRoughness();
	float GetAnisotropy();
	float GetSheen();

	//uint64_t ComputeHash();
	//bool IsValid();
};