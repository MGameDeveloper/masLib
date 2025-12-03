#pragma once

#include <stdint.h>

typedef struct mas_ecs_archtype;


bool mas_ecs_archtype_init();
void mas_ecs_archtype_deinit();


mas_ecs_archtype* mas_ecs_archtype_find(const char** comp_list, size_t comp_count);
mas_ecs_archtype* mas_ecs_archtype_get(uint32_t archtype_idx);

int32_t mas_ecs_archtype_add_entity(mas_ecs_archtype* archtype);
void    mas_ecs_archtype_remove_entity(mas_ecs_archtype* archtype, int32_t entity_idx);
int32_t mas_ecs_archtype_move_entity(mas_ecs_archtype* from, mas_ecs_archtype* to, int32_t entity_idx);

void f()
{
	mas_archtype_query_compes(mas_position, mas_scale, mas_rotation, mas_matrix, mas_scene_node);

	for (int32_t i = 0; i < query_result->count; ++i)
	{
		archtype_query* query = query_result->queries[i];

		mas_position* pos = mas_archtype_query_get(mas_position);
	}
}