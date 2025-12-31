#pragma once

#include "masComponent.h"
#include "masCoreComponents.h"

void RegisterComponents()
{
	masComponent_Register(masVec3);
	masComponent_Register(masPosition);
	masComponent_Register(masRotation);
	masComponent_Register(masScale);
	masComponent_Register(masVelocity);

	masComponent_Register(masVec4);
	masComponent_Register(masQuaternion);

	masComponent_Register(masMatrix4x4);
	masComponent_Register(masWorldMatrix);
	masComponent_Register(masLocalMatrix);

	masComponent_Register(masParent);
	masComponent_Register(masChildren);
}

bool masECS_Init()
{
	if (!masComponent_Init())
		return false;
	if (!masEntity_Init())
		return false;

	RegisterComponents();

	return true;
}

void masECS_DeInit()
{
	masEntity_DeInit();
	masComponent_DeInit();
}