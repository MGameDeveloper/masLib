///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include "mas_ecs_entity.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#define MAS_ECS_ENTITY_DEFAULT_COUNT 4096
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
union mas_entity
{
	uint64_t handle;
	struct
	{
		uint32_t mapper_idx;
		uint32_t gen;
	};
};

struct mas_entity_mapper
{
	uint32_t archtype;
	uint32_t archtype_chunk;
	uint32_t chunk_idx;
	uint32_t gen;
};

struct mas_entity_storage
{
	mas_entity_mapper *list;
	int32_t           *free_indices;
	int32_t            free_count;
	int32_t            mapper_idx;
	int32_t            count;
	int32_t            capacity;
};


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static mas_entity_storage g_mapper = { };


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_ecs_entity_init()
{
	if (g_mapper.list)
		return true;

	uint64_t  mapper_size       = sizeof(mas_entity_mapper) * MAS_ECS_ENTITY_DEFAULT_COUNT;
	uint64_t  free_indices_size = sizeof(int32_t)           * MAS_ECS_ENTITY_DEFAULT_COUNT;
	void     *mem               = malloc(mapper_size + free_indices_size);
	if (!mem)
		return false;
	memset(mem, 0, mapper_size + free_indices_size);

	g_mapper.list         = MAS_PTR_OFFSET(mas_entity_mapper, mem, 0);
	g_mapper.free_indices = MAS_PTR_OFFSET(int32_t,           mem, mapper_size);
	g_mapper.free_count   = 0;
	g_mapper.mapper_idx   = 0;
	g_mapper.count        = 0;
	g_mapper.capacity     = MAS_ECS_ENTITY_DEFAULT_COUNT;

	return true;
}

void mas_ecs_entity_deinit()
{
	if (g_mapper.list)
	{
		free(g_mapper.list);
		memset(&g_mapper, 0, sizeof(mas_entity_storage));
	}
}

mas_ecs_entity_t mas_ecs_entity_create()
{
	mas_entity ent = { 0 };

	if (!g_mapper.list)
		return ent.handle;

	int32_t ent_idx = -1;
	if (g_mapper.mapper_idx < MAS_ECS_ENTITY_DEFAULT_COUNT)
		ent_idx = g_mapper.mapper_idx++;
	else
	{
		if (g_mapper.free_count > 0)
		{
			ent_idx = g_mapper.free_indices[--g_mapper.free_count];
			g_mapper.free_indices[g_mapper.free_count] = -1;
		}
		else
		{
			// resize entity storage and assign a new using mapper_idx
		}
	}

	if (ent_idx == -1)
	{
		// log error
		return false;
	}

	// TODO: use archtype interface to insert the new entity in the default archtype that has all the component to be spawned in the scene
	//       and get entity index from it for faster access to its components later
	mas_entity_mapper* mapper = &g_mapper.list[ent_idx];
	mapper->archtype       = 0;
	mapper->archtype_chunk = 0;
	mapper->chunk_idx      = 0;
	if (mapper->gen == 0)
		mapper->gen = 1;

	ent.mapper_idx = ent_idx;
	ent.gen        = mapper->gen;
	return ent.handle;
}

void mas_ecs_entity_destroy(mas_ecs_entity_t ent)
{

}