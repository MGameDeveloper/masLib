#pragma once

#include "masArchtype.h"
#include "masComponent.h"
#include "masCoreComponents.h"

void RegisterComponents()
{
	MAS_COMPONENT_REGISTER(masVec3);
	MAS_COMPONENT_REGISTER(masPosition);
	MAS_COMPONENT_REGISTER(masRotation);
	MAS_COMPONENT_REGISTER(masScale);
	MAS_COMPONENT_REGISTER(masVelocity);

	MAS_COMPONENT_REGISTER(masVec4);
	MAS_COMPONENT_REGISTER(masQuaternion);

	MAS_COMPONENT_REGISTER(masMatrix4x4);
	MAS_COMPONENT_REGISTER(masWorldMatrix);
	MAS_COMPONENT_REGISTER(masLocalMatrix);

	MAS_COMPONENT_REGISTER(masParent);
	MAS_COMPONENT_REGISTER(masChildren);
}

bool masECS_Init()
{
	if (!masComponent_Init())
		return false;
	if (!masArchtype_Init())
		return false;

	RegisterComponents();

	return true;
}

void masECS_DeInit()
{
	masArchtype_DeInit();
	masComponent_DeInit();
}