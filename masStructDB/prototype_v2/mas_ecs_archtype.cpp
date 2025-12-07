#include "mas_ecs_archtype.h"
#include "mas_ecs_components.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
struct mas_archtype_page
{
	mas_memory_page_id page_id;
	uint32_t           ent_count;
};

struct mas_archtype
{
	mas_memory_array_id page_array;
	mas_memory_array_id page_comp_layout;
	uint64_t            comps_hash;
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

		if (archtype->comps_hash == comp_query->comps_hash)
			return archtype;
	}

	return NULL;
}

mas_archtype* mas_archtype_create(const mas_component_query* comp_query) 
{
	if (!mas_internl_registery_is_valid())
		return NULL;


	// IN CASE OF ERROR IN ANY OF THE STEPS WE MAY NEED TO FIND A WAY TO REUSE IT
	//   NOW WE ADVANCE ARRAY WITHOUT HAVING A TRACKER OF EMPTY SLOTS LIKE IN ENTITY API IMPL
	mas_archtype* archtype = (mas_archtype*)mas_memory_array_new_element(g_reg.archtypes);
	if (!archtype)
	{
		// log error
		return NULL;
	}

	archtype->page_comp_layout = mas_memory_array_create(sizeof(mas_component_query_desc));
	if (!mas_memory_array_is_valid(archtype->page_comp_layout))
	{
		// log error
		return NULL;
	}

	for (uint32_t c = 0; c < comp_query->count; ++c)
	{
		mas_component_query_desc* desc = &comp_query->comps[c];

		mas_component_query_desc* archtype_comp = (mas_component_query_desc*)mas_memory_array_new_element(archtype->page_comp_layout);
		if (!archtype_comp)
		{
			// log error
			return NULL;
		}
		mas_memory_copy(archtype_comp, desc, sizeof(mas_component_query_desc));
	}

	archtype->unique_id        = g_reg.unique_id_gen++;
	archtype->comps_hash       = comp_query->comps_hash;
	archtype->current_page_idx = 0;
	archtype->max_ent_count    = mas_memory_default_page_size() / comp_query->total_comps_size;
	if (archtype->max_ent_count == 0)
	{
		// log error
		return NULL;
	}

	return archtype;

}

const mas_archtype_entity* mas_archtype_new_entity(mas_archtype* archtype) 
{
	if (!archtype)
		return NULL;

	// for first time usage
	if (!mas_memory_array_is_valid(archtype->page_array))
	{
		archtype->page_array = mas_memory_array_create(sizeof(mas_archtype_page));
		if (!mas_memory_array_is_valid(archtype->page_array))
		{
			// log error
			return NULL;
		}

		mas_archtype_page* page = (mas_archtype_page*)mas_memory_array_new_element(archtype->page_array);
		if (!page)
		{
			mas_memory_array_free(archtype->page_array);
			// log erro
			return NULL;
		}

		page->page_id = mas_memory_page_create();
		if (!mas_memory_page_is_valid(page->page_id))
		{
			mas_memory_array_free(archtype->page_array);
			// log erro
			return NULL;
		}

		page->ent_count = 0;
	}

	mas_archtype_page* page = (mas_archtype_page*)mas_memory_array_get_element(archtype->page_array, archtype->current_page_idx);
	if (page->ent_count + 1 >= archtype->max_ent_count)
	{
		page = (mas_archtype_page*)mas_memory_array_new_element(archtype->page_array);
		if (!page)
			return NULL;
		archtype->current_page_idx++;
	}

	mas_archtype_entity *archtype_ent = MAS_FRAME_MEMORY_MALLOC(mas_archtype_entity, sizeof(mas_archtype_entity));
	archtype_ent->archtype_idx = archtype->unique_id;
	archtype_ent->page_idx     = archtype->current_page_idx;
	archtype_ent->entity_idx   = page->ent_count++;

	return archtype_ent;
}

void mas_archtype_remove_entity(const mas_archtype_entity* entity) 
{
	if (!entity)
		return;

	mas_archtype      *archtype = (mas_archtype*)mas_memory_array_get_element(g_reg.archtypes, entity->archtype_idx);
	mas_archtype_page *page     = (mas_archtype_page*)mas_memory_array_get_element(archtype->page_array, entity->page_idx);
	if (!archtype || !page)
		return;

	if (entity->entity_idx >= archtype->max_ent_count)
		return;

	// find a way to remove components
	//   if they have special resources to be free setup an api for it
	// swap it with the last entity to not leave holes in the arrays
}
