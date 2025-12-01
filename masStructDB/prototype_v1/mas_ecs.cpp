#include "mas_ecs_struct_registery.h"
#include "mas_ecs_memory.h"

bool mas_ecs_init()
{
	if (!mas_ecs_memory_init())
		return false;
	if (!mas_struct_registery_init())
		return false;

	return true;
}

void mas_ecs_deinit()
{
	mas_struct_registery_deinit();
	mas_struct_registery_deinit();
}

void mas_ecs_update()
{
	mas_ecs_memory_frame_reset();
}