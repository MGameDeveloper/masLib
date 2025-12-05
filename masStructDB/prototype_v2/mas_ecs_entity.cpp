///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include "mas_ecs_entity.h"
#include "mas_ecs_core_components.h"
#include "mas_ecs_components.h"
#include "mas_ecs_archtype.h"


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
	uint32_t archtype_page_idx;
	uint32_t page_ent_idx;
	uint32_t gen;
};

struct mas_entities
{
	mas_ecs_memory_array_id mappers;
	mas_ecs_memory_stack_id free_indices;
};



///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static mas_entities g_ents = { 0 };



///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static bool mas_internal_are_entities_valid()
{
	if (!mas_ecs_memory_array_is_valid(g_ents.mappers) ||
		!(mas_ecs_memory_stack_is_valid(g_ents.free_indices)))
		return false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_ecs_entity_init()
{
	if (!mas_ecs_memory_array_is_valid(g_ents.mappers))
	{
		g_ents.mappers = mas_ecs_memory_array_create(sizeof(mas_entity_mapper));
		if (!mas_ecs_memory_array_is_valid(g_ents.mappers))
			return false;
	}

	if (!mas_ecs_memory_stack_is_valid(g_ents.free_indices))
	{
		g_ents.free_indices = mas_ecs_memory_stack_create(sizeof(int32_t));
		if (!mas_ecs_memory_stack_is_valid(g_ents.free_indices))
		{
			mas_ecs_memory_array_free(g_ents.mappers);
			return false;
		}
	}

	return true;
}

void mas_ecs_entity_deinit()
{
	mas_ecs_memory_array_free(g_ents.mappers);
	mas_ecs_memory_stack_free(g_ents.free_indices);
}

mas_ecs_entity mas_ecs_entity_create()
{
	if (!mas_internal_are_entities_valid())
		return { 0 };

	int32_t           *entity_mapper_idx_ptr = NULL;
	mas_entity_mapper *entity_mapper         = (mas_entity_mapper*)mas_ecs_memory_array_new_element(g_ents.mappers);
	if (!entity_mapper)
	{
		if (!mas_ecs_memory_stack_is_empty(g_ents.free_indices))
		{
			entity_mapper_idx_ptr = (int32_t*)mas_ecs_memory_stack_top_element(g_ents.free_indices);
			if (entity_mapper_idx_ptr)
			{
				entity_mapper = (mas_entity_mapper*)mas_ecs_memory_array_get_element(g_ents.mappers, *entity_mapper_idx_ptr);
				mas_ecs_memory_stack_pop_element(g_ents.free_indices);
			}
			else
				return { 0 };
		}
		else
		{
			if (!mas_ecs_memory_array_resize(g_ents.mappers))
				return { 0 };

			entity_mapper = (mas_entity_mapper*)mas_ecs_memory_array_new_element(g_ents.mappers);
			if (!entity_mapper)
				return { 0 };
		}
	}

	// in case of failur
	//mas_ecs_memory_stack_push_element(g_ents.free_indices, entity_mapper_idx_ptr, sizeof(int32_t));


#if 0
	// TODO: use archtype interface to insert the new entity in the default archtype that has all the component to be spawned in the scene
	//       and get entity index from it for faster access to its components later
	MAS_COMPONENT_QUERY_LIST(default_comps, 
		MAS_COMP(mas_position), 
		MAS_COMP(mas_rotation), 
		MAS_COMP(mas_scale), 
		MAS_COMP(mas_matrix), 
		MAS_COMP(mas_scene_node));
	
	mas_archtype* archtype = mas_ecs_archtype_find(default_comps);
	if (!archtype)
	{
		archtype = mas_ecs_archtype_create(default_comps);
		if (!archtype)
			return { 0 };
	}

	mas_archtype_entity *new_ent = mas_ecs_archtype_new_entity(archtype);
	if (!new_ent)
		return { 0 };

	mas_entity_mapper* mapper = &g_mapper.list[ent_idx];
	mapper->archtype          = new_ent->archtype_unique_id;
	mapper->archtype_page_idx = new_ent->archtype_page_idx;
	mapper->page_ent_idx      = new_ent->page_entity_idx;
	if (mapper->gen == 0)
		mapper->gen = 1;

	mas_entity ent = { 0 };
	ent.mapper_idx = ent_idx;
	ent.gen        = mapper->gen;
	return { ent.handle };
#endif

	return { 0 };
}

void mas_ecs_entity_destroy(mas_ecs_entity ent_id)
{

}