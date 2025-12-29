#pragma once

#include <stdint.h>



struct masEntity
{
	uint64_t Handle;
};

masEntity masEntity_Create(); // would create default entity that can be placed in the scene and can be linked to other entities
void*     masEntity_AddComponentByName(masEntity Entity, const char* CompName); // this would work only if components already registered
void*     masEntity_GetComponentByName(masEntity Entity, const char* CompName);

uint32_t  masEntity_ChildCount (masEntity Parent);
masEntity masEntity_GetChild   (masEntity Parent, uint32_t ChildIdx);
masEntity masEntity_FindChild  (masEntity Parent, const char* ChildName);
void      masEntity_RemoveChild(masEntity Parent, masEntity Child);
void      masEntity_AddChild   (masEntity Parent, masEntity Child);

#define masEntity_AddComponent(Entity, Component) masEntity_AddComponentByName(Entity, #Component)
#define masEntity_GetComponent(Entity, Component) masEntity_GetComponentByName(Entity, #Component)