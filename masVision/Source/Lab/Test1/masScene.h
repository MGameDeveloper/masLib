#pragma once

#include "Renderer/masRender.h"
#include <vector>
#include <memory>


/***********************************************************************************
*
************************************************************************************/
struct masModel;
struct masLight;


/***********************************************************************************
*
************************************************************************************/
struct masVec3
{
	float x, y, z;
	masVec3(float x, float y, float z) : x(x), y(y), z(z) { }
};


struct masEntity
{
	union masData
	{
		masModel* Model;
		masLight* Light;
		void* Ptr;

		masData(void* InPtr) : Ptr(InPtr) { }
	} Data;

	masVec3 Position;
	masVec3 Rotation;
	masVec3 Scale;

	explicit masEntity(void* Data) :
		Data(Data),
		Position(0.f, 0.f, 0.f),
		Rotation(0.f, 0.f, 0.f),
		Scale(1.f, 1.f, 1.f)
	{
	}

	XMMATRIX GetWorldMatrix()
	{
		XMMATRIX World = XMMatrixIdentity();
		World *= XMMatrixScalingFromVector(XMLoadFloat3((XMFLOAT3*)&Scale));
		World *= XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3((XMFLOAT3*)&Rotation));
		World *= XMMatrixTranslationFromVector(XMLoadFloat3((XMFLOAT3*)&Position));
		return World;
	}
};


/***********************************************************************************
*
************************************************************************************/
struct masScene
{
	masRender GRender;
	
	std::vector<std::unique_ptr<masEntity>> Models;
	std::vector<std::unique_ptr<masEntity>> Lights;
	std::vector<std::unique_ptr<masCamera>> Cameras;
	std::vector<std::shared_ptr<masShader>> Shaders;

	bool Init();
	void Terminate();
	void AddModel(masModel* Model);
	void AddLight(masLight* Light);
	void Render();
};

