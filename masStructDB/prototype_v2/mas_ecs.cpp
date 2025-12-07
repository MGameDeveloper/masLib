///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include "mas_ecs_archtype.h"
#include "mas_ecs.h"

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_ecs_init()
{
    if(!mas_memory_init())
        return false;
    if(!mas_ecs_components_init())
        return false;
    if (!mas_entity_init())
        return false;
    if (!mas_archtype_init())
        return false;
    mas_ecs_core_components_register();

    return true;
}

void mas_ecs_deinit()
{
    mas_ecs_components_deinit();
    mas_entity_deinit();
    mas_archtype_deinit();
    mas_memory_deinit();
}

void mas_ecs_update()
{
    mas_memory_frame_reset();
}