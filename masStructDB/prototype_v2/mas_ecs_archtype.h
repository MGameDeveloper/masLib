#pragma once

#include "mas_ecs_memory.h"


struct mas_archtype_entity
{
	uint32_t archtype_unique_id;
	uint32_t archtype_page_idx;
	uint32_t page_entity_idx;
};

struct mas_archtype_chunk
{
	mas_ecs_memory_page_id page_id;
	uint32_t               ent_count;
};

struct mas_archtype
{
	mas_ecs_memory_array_id page_array;
	mas_ecs_memory_array_id page_comp_layout;
	uint32_t                max_ent_count;
	uint32_t                unique_id;
	uint32_t                current_page_idx;
};

struct mas_archtype_registery
{
	mas_ecs_memory_array_id archtype_array;
	uint32_t                unique_id_gen;
};


