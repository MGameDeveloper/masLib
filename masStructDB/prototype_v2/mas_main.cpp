///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdint.h>
#include "mas_ecs.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
union mas_vec2
{
    float xy[2];
    struct 
    {
        float x, y;
    };
};

typedef union mas_vec3
{
    float xyz[3];
    struct
    {
        float x, y, z;
    };
} mas_position, mas_velocity, mas_rotation, mas_scale;

typedef union mas_vec4
{
    float xyzw[4];
    struct 
    { 
        float x, y, z, w; 
    };
} mas_quaternion;

union mas_matrix
{
    float m16[16];
};


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static void mas_register_components()
{
    MAS_COMPONENT_REGISTER(mas_vec2);

    MAS_COMPONENT_REGISTER(mas_vec3);
    MAS_COMPONENT_REGISTER(mas_position);
    MAS_COMPONENT_REGISTER(mas_velocity);
    MAS_COMPONENT_REGISTER(mas_rotation);
    MAS_COMPONENT_REGISTER(mas_scale);

    MAS_COMPONENT_REGISTER(mas_vec4);
    MAS_COMPONENT_REGISTER(mas_quaternion);

    MAS_COMPONENT_REGISTER(mas_matrix);

    // FOR DEBUG TO ENSURE REGISTERATION
    mas_ecs_comonents_print();
}

void mas_test_components_query()
{
    MAS_COMPONENT_QUERY_LIST(transform_comps,
        MAS_COMP(mas_position),
        MAS_COMP(mas_rotation),
        MAS_COMP(mas_scale),
        MAS_COMP(mas_matrix));
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
    
    mas_register_components();
    mas_test_components_query();

    while(1)
    {
        mas_ecs_update();
    }

    mas_ecs_deinit();

    return 0;
}