#pragma once

#include "mas_ecs_memory.h"
#include "mas_ecs_components.h"

struct mas_archtype_chunk
{
	mas_ecs_memory_t page;
	uint32_t         ent_count;
};

struct mas_archtype
{
	mas_archtype_chunk  *chunks;
	mas_component_query *layout;
	uint64_t             unique_id;
	uint32_t             current_chunk;
	uint32_t             max_ent_count;
};

struct mas_archtype_registery
{
	mas_archtype *archtypes;
	uint32_t      count;
};


