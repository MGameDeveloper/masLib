#pragma once


struct mas_archtype_entity
{
	uint32_t archtype_idx;
	uint32_t page_idx;
	uint32_t entity_idx;
};


// forward declarations
struct mas_component_query; 
struct mas_archtype;

bool                       mas_archtype_init();
void                       mas_archtype_deinit();
mas_archtype*              mas_archtype_find(const mas_component_query* comp_query);
mas_archtype*              mas_archtype_create(const mas_component_query* comp_query);
const mas_archtype_entity* mas_archtype_new_entity(mas_archtype* archtype);
void                       mas_archtype_remove_entity(const mas_archtype_entity* entity);
