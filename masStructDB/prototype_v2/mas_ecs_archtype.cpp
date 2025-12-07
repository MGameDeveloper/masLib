#include "mas_ecs_archtype.h"
#include "mas_ecs_components.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
struct mas_archtype_comp
{

};

struct mas_archtype_page
{
	mas_memory_page_id page_id;
	uint32_t           ent_count;
};

struct mas_archtype
{
	mas_memory_array_id page_array;
	mas_memory_array_id page_comp_layout;
	uint32_t            max_ent_count;
	uint32_t            unique_id;
	uint32_t            current_page_idx;
};

struct mas_archtype_registery
{
	mas_memory_array_id archtypes;
	uint32_t            unique_id_gen;
};


///////////////////////////////////////////////////////////////////////////////////////
// INTERNAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////////
static mas_archtype_registery g_reg = { 0 };


///////////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////
static bool mas_internl_registery_is_valid()
{
	if (!mas_memory_array_is_valid(g_reg.archtypes))
		return false;
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_archtype_init()
{
	if (mas_internl_registery_is_valid())
		return true;

	g_reg.archtypes = mas_memory_array_create(sizeof(mas_archtype));
	if (!mas_internl_registery_is_valid())
		return false;

	return true;
}

void mas_archtype_deinit()
{
	// NOTE: some component in the page may have its resources to be freed individually
	//           what if every component has its pointer to descrutor that knowns how to free
	//           itself ( WILL BE INVISTIGATED LATER THIS VERSION IS JUST A PROTOTYPE)
	if (mas_internl_registery_is_valid())
	{
		// loop through all the archtypes in the registery
		size_t archtype_count = mas_memory_array_element_count(g_reg.archtypes);
		for (size_t a = 0; a < archtype_count; ++a)
		{
			mas_archtype* archtype = (mas_archtype*)mas_memory_array_get_element(g_reg.archtypes, a);
			if (archtype)
			{
				// free all pages and comp_layout of an archtype
				size_t page_count = mas_memory_array_element_count(archtype->page_array);
				for (size_t p = 0; p < page_count; ++p)
				{
					mas_archtype_page* page = (mas_archtype_page*)mas_memory_array_get_element(archtype->page_array, p);
					if (page)
						mas_memory_page_free(page->page_id);
				}
				mas_memory_array_free(archtype->page_comp_layout);
			}
		}
		mas_memory_array_free(g_reg.archtypes);

		mas_memory_zero(&g_reg, sizeof(mas_archtype_registery));
	}
}

mas_archtype* mas_archtype_find(const mas_component_query* comp_query)
{
	if (!mas_internl_registery_is_valid())
		return NULL;

	size_t archtype_count = mas_memory_array_element_count(g_reg.archtypes);
	if (archtype_count == 0)
		return NULL;

	for (size_t a = 0; a < archtype_count; ++a)
	{
		mas_archtype* archtype = (mas_archtype*)mas_memory_array_get_element(g_reg.archtypes, a);
		if (!archtype)
			continue;

		if (archtype->comps_hash == comp_query->hash)
			return archtype;
#if 0
		size_t comp_count = mas_memory_array_element_count(archtype->page_comp_layout);
		if (comp_count == 0)
			continue;

		uint64_t comps_hash = 0;
		for (size_t c = 0; c < comp_count; ++c)
		{
			mas_archtype_comp* comp = (mas_archtype_comp*)mas_memory_array_get_element(archtype->page_comp_layout, c);
			if (!comp)
			{
				// calculate hash
			}
		}
#endif
	}

	return NULL;
}

mas_archtype* mas_archtype_create(const mas_component_query* comp_query) 
{
}

const mas_archtype_entity* mas_archtype_new_entity(mas_archtype* archtype) 
{
}

void mas_archtype_remove_entity(const mas_archtype_entity* entity) 
{
}
