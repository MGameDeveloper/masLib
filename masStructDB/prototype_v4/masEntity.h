#pragma once

#include <stdint.h>

typedef uint64_t masEntity;

masEntity masEntity_Create(const char* Components = NULL);
void      masEntity_Destroy(masEntity Entity);
void*     masEntity_AddComponentByName(masEntity Entity, const char* CompName);
void      masEntity_RemoveComponentByName(masEntity Entity, const char* CompName);
void*     masEntity_GetComponentByName(masEntity Entity, const char* CompName);
void      masEntity_AddChild(masEntity Parent, masEntity Child);
void      masEntity_RemoveChild(masEntity Child);

#define MAS_ENTITY_ADD_COMPONENT(Entity, Component)    (Component*)masEntity_AddComponentByName(Entity, #Component)
#define MAS_ENTITY_REMOVE_COMPONENT(Entity, Component)             masEntity_RemoveComponentByName(Entity, #Component)
#define MAS_ENTITY_GET_COMPONENT(Entity, Component)    (Component*)masEntity_GetComponentByName(Entity, #Component)
