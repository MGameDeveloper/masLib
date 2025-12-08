#include "mas_ecs_archtype.h"
#include "mas_ecs_components.h"

#include "utils/mas_array.h"
#include "utils/mas_stack.h"
#include "utils/mas_page.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
struct mas_archtype_page
{
	mas_page<void> page;
	uint32_t       ent_count;
};

struct mas_archtype
{
	mas_array<mas_archtype_page>        pages;
	mas_array<mas_component_query_desc> page_layout;

	uint64_t            comps_hash;
	uint32_t            max_ent_count;
	uint32_t            unique_id;
	uint32_t            current_page_idx;
};

struct mas_archtype_registery
{
	mas_array<mas_archtype> archtypes;
	uint32_t                unique_id_gen;
};

class mas_archtype_tracker
{
private:
	mas_array<mas_archtype> archtypes;
	mas_stack<uint32_t>     free_indices;
	uint32_t                unique_id_gen;

public:
	mas_archtype_tracker(const mas_archtype_tracker&)            = delete;
	mas_archtype_tracker(mas_archtype_tracker&&)                 = delete;
	mas_archtype_tracker& operator=(const mas_archtype_tracker&) = delete;
	mas_archtype_tracker& operator=(mas_archtype_tracker&)       = delete;


public:
	mas_archtype_tracker() : unique_id_gen(0) 
	{ 
	}

	~mas_archtype_tracker()
	{
		destroy(); 
	}

	bool create()
	{
		if (!archtypes.create())
			return false;
		if (!free_indices.create())
		{
			archtypes.destroy();
			return false;
		}
		return true;
	}

	void destroy()
	{
		// NOTE: some component in the page may have its resources to be freed individually
        //        what if every component has its pointer to descrutor that knowns how to free
        //        itself ( WILL BE INVISTIGATED LATER THIS VERSION IS JUST A PROTOTYPE)
		for (size_t a = 0; a < archtypes.count(); ++a)
		{
			mas_archtype* archtype = archtypes.get(a);
			if (archtype)
			{
				for (size_t p = 0; p < archtype->pages.count(); ++p)
				{
					mas_archtype_page* archtype_page = archtype->pages.get(p);
					if (archtype_page)
						archtype_page->page.destroy();
				}
				archtype->page_layout.destroy();
			}
		}
		archtypes.destroy();
		free_indices.destroy();

		mas_memory_zero(this, sizeof(*this));
	}

	mas_archtype* find(const mas_component_query* comp_query)
	{
		if (g_reg.archtypes.is_empty())
			return NULL;

		for (size_t a = 0; a < g_reg.archtypes.count(); ++a)
		{
			mas_archtype* archtype = g_reg.archtypes.get(a);
			if (!archtype)
				continue;

			if (archtype->comps_hash == comp_query->comps_hash)
				return archtype;
		}

		return NULL;
	}

	mas_archtype* add(const mas_component_query* comp_query)
	{
		// IN CASE OF ERROR IN ANY OF THE STEPS WE MAY NEED TO FIND A WAY TO REUSE IT
	    //   NOW WE ADVANCE ARRAY WITHOUT HAVING A TRACKER OF EMPTY SLOTS LIKE IN ENTITY API IMPL
		mas_archtype archtype = { };

		if (!archtype.page_layout.create())
			return NULL;

		for (uint32_t c = 0; c < comp_query->count; ++c)
			archtype.page_layout.add(&comp_query->comps[c]);

		// get any privously free arctypes
		if (!free_indices.is_empty())
		{
			uint32_t* ptr = free_indices.top();
			if (ptr)
			{
				archtype.unique_id = *ptr;
				free_indices.pop();
			}
			else
				archtype.unique_id = unique_id_gen++;
		}
		else
			archtype.unique_id = unique_id_gen++;

		archtype.comps_hash       = comp_query->comps_hash;
		archtype.current_page_idx = 0;
		archtype.max_ent_count    = mas_memory_default_page_size() / comp_query->total_comps_size;
		if (archtype.max_ent_count == 0)
		{
			archtype.page_layout.destroy();
			free_indices.push(&archtype.unique_id);

			// log error
			return NULL;
		}

		archtypes.add(&archtype);
		return archtypes.get(archtype.unique_id);
	}

	void remove()
	{

	}
};


///////////////////////////////////////////////////////////////////////////////////////
// INTERNAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////////
static mas_archtype_registery g_reg = { };


///////////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_archtype_init()
{
	if (!g_reg.archtypes.create())
		return false;
	
	return true;
}

void mas_archtype_deinit()
{

}

mas_archtype* mas_archtype_find(const mas_component_query* comp_query)
{

}

mas_archtype* mas_archtype_create(const mas_component_query* comp_query) 
{
	if (g_reg.archtypes.is_empty())
		return NULL;


	// IN CASE OF ERROR IN ANY OF THE STEPS WE MAY NEED TO FIND A WAY TO REUSE IT
	//   NOW WE ADVANCE ARRAY WITHOUT HAVING A TRACKER OF EMPTY SLOTS LIKE IN ENTITY API IMPL
	mas_archtype archtype = { };

	if (!archtype.page_layout.create())
		return NULL;

	for (uint32_t c = 0; c < comp_query->count; ++c)
		archtype.page_layout.add(&comp_query->comps[c]);

	archtype.unique_id        = g_reg.unique_id_gen++;
	archtype.comps_hash       = comp_query->comps_hash;
	archtype.current_page_idx = 0;
	archtype.max_ent_count    = mas_memory_default_page_size() / comp_query->total_comps_size;
	if (archtype.max_ent_count == 0)
	{
		archtype.page_layout.destroy();

		// log error
		return NULL;
	}

	g_reg.archtypes.add(&archtype);	
	return g_reg.archtypes.get(g_reg.archtypes.count() - 1);

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
