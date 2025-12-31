#pragma once

#include <stdint.h>

typedef uint64_t masEntity;

masEntity masEntity_Create();
void      masEntity_Destroy(masEntity Entity);
void*     masEntity_AddComponentByName(masEntity Entity, const char* CompName);
void*     masEntity_GetComponentByName(masEntity Entity, const char* CompName);
//void      masEntity_AddChild(masEntity Parent, masEntity Child);
//void      masEntity_RemoveChild(masEntity Child);

#define masEntity_AddComponent(Entity, Component) (Component*)masEntity_AddComponentByName(Entity, #Component)
#define masEntity_GetComponent(Entity, Component) (Component*)masEntity_GetComponentByName(Entity, #Component)
