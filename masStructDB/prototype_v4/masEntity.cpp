#include <stdio.h>
#include <string.h>

#include "masEntity.h"
#include "masArchtype.h"
#include "masCoreComponents.h"
#include "masComponent.h"


masEntity masEntity_Create(const char* Components)
{
	masEntity Entity = masArchtype_CreateEntity(Components);
	return Entity;
}

void masEntity_Destroy(masEntity Entity)
{
	masArchtype_DestroyEntity(Entity);
}

void* masEntity_AddComponentByName(masEntity Entity, const char* CompName)
{
	return masArchtype_AddEntityComponent(Entity, CompName);
}

void masEntity_RemoveComponentByName(masEntity Entity, const char* CompName)
{
	masArchtype_RemoveEntityComponent(Entity, CompName);
}

void* masEntity_GetComponentByName(masEntity Entity, const char* CompName)
{
	return masArchtype_GetEntityComponent(Entity, CompName);
}

void masEntity_AddChild(masEntity Parent, masEntity Child)
{
	masArchtype_AddEntityChild(Parent, Child);
}

void masEntity_RemoveChild(masEntity Child)
{
	masArchtype_RemoveEntityChild(Child);
}