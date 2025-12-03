///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include "mas_ecs_core_components.h"
#include "mas_ecs_components.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void mas_ecs_core_components_register()
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