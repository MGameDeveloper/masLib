#include <stdio.h>
#include <string.h>

#include "masEntity.h"
#include "masArchtype.h"
#include "masCoreComponents.h"
#include "masComponent.h"


masEntity masEntity_Create()
{
	masComponentList(DefaultComponents, masPosition, masRotation, masScale, masWorldMatrix, masLocalMatrix, masParent);
	masEntity Entity = masArchtype_CreateEntity(DefaultComponents);
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

void* masEntity_GetComponentByName(masEntity Entity, const char* CompName)
{
	return masArchtype_GetEntityComponent(Entity, CompName);
}