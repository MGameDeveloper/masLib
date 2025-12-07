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
union mas_entity_handle
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
	uint32_t archtype_idx;
	uint32_t page_idx;
	uint32_t entity_idx;
	uint32_t gen;
};

struct mas_entities
{
	mas_memory_array_id mappers;
	mas_memory_stack_id free_indices;
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
	if (!mas_memory_array_is_valid(g_ents.mappers) ||
		!(mas_memory_stack_is_valid(g_ents.free_indices)))
		return false;

	return true;
}

static void mas_internal_put_mapper_idx_back(int32_t mapper_idx)
{
	if (mapper_idx < 0)
		return;

	int32_t* ptr = (int32_t*)mas_memory_stack_push_element(g_ents.free_indices);
	if (!ptr)
		return;

	*ptr = mapper_idx;
}


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_entity_init()
{
	if (!mas_memory_array_is_valid(g_ents.mappers))
	{
		g_ents.mappers = mas_memory_array_create(sizeof(mas_entity_mapper));
		if (!mas_memory_array_is_valid(g_ents.mappers))
			return false;
	}

	if (!mas_memory_stack_is_valid(g_ents.free_indices))
	{
		g_ents.free_indices = mas_memory_stack_create(sizeof(int32_t));
		if (!mas_memory_stack_is_valid(g_ents.free_indices))
		{
			mas_memory_array_free(g_ents.mappers);
			return false;
		}
	}

	return true;
}

void mas_entity_deinit()
{
	mas_memory_array_free(g_ents.mappers);
	mas_memory_stack_free(g_ents.free_indices);
}

mas_entity mas_entity_create()
{
	if (!mas_internal_are_entities_valid())
		return { 0 };

	// Get previouse freed index or add new a new one
	int32_t            mapper_idx = -1;
	mas_entity_mapper *ent_mapper = NULL;
	if (!mas_memory_stack_is_empty(g_ents.free_indices))
	{
		int32_t *mapper_idx_ptr = (int32_t*)mas_memory_stack_top_element(g_ents.free_indices);
		if (mapper_idx_ptr)
		{
			mapper_idx = *mapper_idx_ptr;
			ent_mapper = (mas_entity_mapper*)mas_memory_array_get_element(g_ents.mappers, mapper_idx);
			if (ent_mapper)
				mas_memory_stack_pop_element(g_ents.free_indices);
		}
	}
	else
		ent_mapper = (mas_entity_mapper*)mas_memory_array_new_element(g_ents.mappers);

	if (!ent_mapper)
	{
		// log error
		return { 0 };
	}

	// TODO: use archtype interface to insert the new entity in the default archtype that has all the component to be spawned in the scene
	//       and get entity index from it for faster access to its components later
	// THIS SHOULD SORT IDs INTERNALLY SO NO MATTER HOW MIX THEM THEY SHOULD GAVE THE SAME HASH VALUE TO BE USED LATER FOR ARCHTYPE QUERY AND CREATION
	MAS_COMPONENT_QUERY_LIST(default_comps, 
		MAS_COMP(mas_position), 
		MAS_COMP(mas_rotation), 
		MAS_COMP(mas_scale), 
		MAS_COMP(mas_matrix), 
		MAS_COMP(mas_scene_node));
	
	mas_archtype *archtype = mas_archtype_find(default_comps);
	if (!archtype)
		archtype = mas_archtype_create(default_comps);

	const mas_archtype_entity *archtype_entity = mas_archtype_new_entity(archtype);
	if (!archtype_entity)
	{
		mas_internal_put_mapper_idx_back(mapper_idx);
		// log error & return invalid entity handle
		return { 0 };
	}
	
	ent_mapper->archtype_idx = archtype_entity->archtype_idx;
	ent_mapper->page_idx     = archtype_entity->page_idx;
	ent_mapper->entity_idx   = archtype_entity->entity_idx;
	if (ent_mapper->gen == 0)
		ent_mapper->gen = 1;

	mas_entity_handle ent_handle = { 0 };
	ent_handle.mapper_idx = mapper_idx;
	ent_handle.gen        = ent_mapper->gen;
	return { ent_handle.handle };
}

void mas_entity_destroy(mas_entity ent_id)
{
	if (!mas_internal_are_entities_valid())
		return;

	mas_entity_handle ent_handle = { ent_id.id };
	if (ent_handle.gen == 0 || (ent_handle.mapper_idx >= mas_memory_array_element_count(g_ents.mappers)))
		return;

	mas_entity_mapper* ent_mapper = (mas_entity_mapper*)mas_memory_array_get_element(g_ents.mappers, ent_handle.mapper_idx);
	if (!ent_mapper || (ent_mapper->gen != ent_handle.gen))
		return;


	ent_mapper->gen++;
	if (ent_mapper->gen == 0)
		ent_mapper->gen = 1;
	mas_internal_put_mapper_idx_back(ent_handle.mapper_idx);

	mas_archtype_entity archtype_ent = { };
	archtype_ent.archtype_idx = ent_mapper->archtype_idx;
	archtype_ent.page_idx     = ent_mapper->page_idx;
	archtype_ent.entity_idx   = ent_mapper->entity_idx;

	mas_archtype_remove_entity(&archtype_ent);
}