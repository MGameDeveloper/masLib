#pragma once

#include "mas_entity.h"


// forward declarations
struct mas_component_query; 
struct mas_archtype;

bool          mas_archtype_init();
void          mas_archtype_deinit();
mas_archtype* mas_archtype_find(const mas_component_query* comp_query);
mas_archtype* mas_archtype_create(const mas_component_query* comp_query);
void          mas_archtype_destroy(mas_archtype* archtype);
mas_entity    mas_archtype_new_entity(mas_archtype* archtype);
void          mas_archtype_remove_entity(mas_entity entity);
mas_archtype* mas_archtype_get_by_entity(mas_entity entity, mas_component_query* comp_query);