///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdint.h>
#include "mas_ecs.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void mas_test_components_query()
{
    // TODO: components_query used to find an archtype or create archtype
    //          after adding components to entity
    MAS_COMPONENT_QUERY_LIST(transform_comps,
        MAS_COMP(mas_position),
        MAS_COMP(mas_rotation),
        MAS_COMP(mas_scale),
        MAS_COMP(mas_matrix));
    // archtype* a = mas_ecs_archtype_query(transform_comps)
    if(!transform_comps)
    {
        printf("ERROR: [ TRANSFORM_COMPS_NOT_FOUND ]\n");
    }
    else
    {
        printf("\nTRANSFORM_COMPS_IDS:\n");
        for(int32_t i = 0; i < transform_comps->count; ++i)
            printf("    - %s: %u\n", transform_comps_query_list[i], transform_comps->ids[i]);
        printf("\n");
    }
}


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char** argv)
{
    if(!mas_ecs_init())
        return -1;
    
    mas_test_components_query();

    while(1)
    {
        mas_ecs_update();
    }

    mas_ecs_deinit();

    return 0;
}