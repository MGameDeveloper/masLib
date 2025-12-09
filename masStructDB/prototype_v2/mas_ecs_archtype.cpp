#include "mas_ecs_archtype.h"
#include "mas_ecs_components.h"

#include "utils/mas_array.h"
#include "utils/mas_stack.h"
#include "utils/mas_page.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
union mas_entity_handle
{
	uint64_t handle;
	struct
	{
		uint32_t mapper_idx;
		uint32_t gen;
	};
};

struct mas_entity_mapper
{
	uint32_t archtype_idx;
	uint32_t page_idx;
	uint32_t entity_idx;
	uint32_t gen;
};

struct mas_entity_new
{
	mas_entity_mapper *mapper;
	mas_entity_handle  handle;
};

struct mas_entities
{
	mas_array<mas_entity_mapper> mappers;
	mas_stack<uint32_t>          free_mappers_ids;

	bool is_valid()
	{
		return (mappers.is_valid() && free_mappers_ids.is_valid());
	}

	bool init()
	{
		if (!mappers.create())
			return false;
		if (!free_mappers_ids.create())
		{
			mappers.destroy();
			return false;
		}
		return true;
	}

	void deinit()
	{
		mappers.destroy();
		free_mappers_ids.destroy();
	}

	mas_entity_new create_entity()
	{
		mas_entity_new new_ent = { };

		if (!is_valid())
			return new_ent;

		uint32_t           ent_mapper_idx = 0;
		mas_entity_handle  ent_handle     = { };
		mas_entity_mapper *ent_mapper     = NULL;

		if (free_mappers_ids.is_empty())
		{
			ent_mapper = mappers.new_element();
			if (!ent_mapper)
				return new_ent;
			
			ent_mapper_idx = mappers.count() - 1;
		}
		else
		{
			uint32_t* ent_mapper_idx_ptr = free_mappers_ids.top();
			if (!ent_mapper_idx_ptr)
				return new_ent;

			uint32_t mapper_idx = *ent_mapper_idx_ptr;
			ent_mapper = mappers.get_element(mapper_idx);
			if (!ent_mapper)
				return new_ent;

			free_mappers_ids.pop();
		}

		if (ent_mapper->gen == 0)
			ent_mapper->gen = 1;

		ent_handle.mapper_idx = ent_mapper_idx;
		ent_handle.gen        = ent_mapper->gen;

		new_ent.handle = ent_handle;
		new_ent.mapper = ent_mapper;
		return new_ent;
	}

	void destroy_entity(mas_entity_handle ent_handle)
	{
		if (!is_valid() || ent_handle.gen == 0)
			return;
		
		mas_entity_mapper* ent_mapper = mappers.get_element(ent_handle.mapper_idx);
		ent_mapper->gen++;
		if (ent_mapper->gen == 0)
			ent_mapper->gen = 1;

		free_mappers_ids.push(&ent_handle.mapper_idx);
	}
};


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static mas_entities g_ents = { };


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
struct mas_archtype_page
{
	mas_page<void>               page;
	uint32_t                     ent_count;
};

struct mas_archtype
{
	mas_array<mas_archtype_page>        pages;
	mas_stack<uint32_t>                 free_indices;
	mas_array<mas_component_query_desc> page_layout;
	uint64_t comps_hash;
	uint32_t max_ent_count;
	uint32_t unique_id;
	uint32_t last_page_idx;

	const mas_entity_handle create_entity()
	{
		mas_entity_handle dumb_handle = { 0 };

		uint32_t           page_idx = last_page_idx;
		mas_archtype_page *page     = NULL;

		if (!pages.is_valid())
			pages.create(2);

		if (free_indices.is_empty())
		{
			page = pages.get_element(page_idx);
			if (!page || (page->ent_count >= max_ent_count))
			{
				page = pages.new_element();
				if (!page)
					return dumb_handle;

				size_t page_count = pages.count();
				if (page_count > 0)
					last_page_idx = page_count - 1;

				page_idx = last_page_idx;
			}
		}
		else
		{
			uint32_t* page_idx_ptr = free_indices.top();
			if (!page_idx_ptr)
				return dumb_handle;

			page_idx = *page_idx_ptr;
			page     =  pages.get_element(page_idx);
			if (!page)
				return dumb_handle;

			if ((page->ent_count + 1) >= max_ent_count)
				free_indices.pop();
		}

		if (!page->page.is_valid())
			page->page.create();

		mas_entity_new new_ent       = g_ents.create_entity();
		new_ent.mapper->archtype_idx = unique_id;
		new_ent.mapper->page_idx     = page_idx;
		new_ent.mapper->entity_idx   = page->ent_count++;

		return new_ent.handle;
	}

	void destroy_entity(mas_entity_handle ent_handle)
	{
		if (ent_handle.gen == 0)
			return;

		mas_entity_mapper* ent_mapper = g_ents.mappers.get_element(ent_handle.mapper_idx);
		if (ent_mapper->gen != ent_handle.gen || ent_mapper->archtype_idx != unique_id)
			return;

		mas_archtype_page* page = pages.get_element(ent_mapper->page_idx);
		if (!page || (page->ent_count == 0))
			return;

		uint32_t           last_ent_idx    = page->ent_count - 1;
		mas_entity_mapper *last_ent_mapper = NULL;
		if (last_ent_idx != ent_mapper->entity_idx)
		{
			last_ent_mapper = g_ents.mappers.get_element(last_ent_idx);
			if (!last_ent_mapper)
			{
				// log error
				return;
			}
		}
		page->ent_count--;

		uint32_t ent_idx           = ent_mapper->entity_idx;
		size_t   comp_array_offset = 0;
		void    *page_data         = page->page.data();
		for (size_t c = 0; c < page_layout.count(); ++c)
		{
			mas_component_query_desc *comp_desc = page_layout.get_element(c);

			uint32_t  ent_comp_idx = comp_array_offset + (comp_desc->size * ent_idx);
			void     *ent_comp     = MAS_PTR_OFFSET(void, page_data, ent_comp_idx);

			// TODO: component may have resources handle find a solution so deleting an object would free them or decreament reference

			if (last_ent_mapper)
			{
				uint32_t  last_ent_comp_idx = comp_array_offset + (comp_desc->size * last_ent_idx);
				void     *last_ent_comp     = MAS_PTR_OFFSET(void, page_data, last_ent_comp_idx);

				mas_memory_copy(ent_comp, last_ent_comp, comp_desc->size);
			}
			else
			{
				mas_memory_zero(ent_comp, comp_desc->size);
			}


			comp_array_offset += comp_desc->size * max_ent_count;
		}

		// updated last entity mapper since we moved in place of entity we just destroyed to keep it dense
		if (last_ent_mapper)
			last_ent_mapper->entity_idx = ent_idx;

		g_ents.destroy_entity(ent_handle);
	}
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
	mas_archtype_tracker() 
		: unique_id_gen(0) 
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
			mas_archtype* archtype = archtypes.get_element(a);
			if (archtype)
			{
				for (size_t p = 0; p < archtype->pages.count(); ++p)
				{
					mas_archtype_page* archtype_page = archtype->pages.get_element(p);
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
		if (archtypes.is_empty())
			return NULL;

		for (size_t a = 0; a < archtypes.count(); ++a)
		{
			mas_archtype* archtype = archtypes.get_element(a);
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

		if (!archtype.free_indices.is_valid())
			archtype.free_indices.create();

		archtype.comps_hash     = comp_query->comps_hash;
		archtype.last_page_idx  = 0;
		archtype.max_ent_count  = mas_memory_default_page_size() / comp_query->total_comps_size;
		if (archtype.max_ent_count == 0)
		{
			archtype.page_layout.destroy();
			archtype.free_indices.destroy();
			free_indices.push(&archtype.unique_id);

			// log error
			return NULL;
		}

		archtypes.add(&archtype);
		return archtypes.get_element(archtype.unique_id);
	}

	mas_archtype* get_archtype(uint32_t idx)
	{
		return archtypes.get_element(idx);
	}

	void remove()
	{
		// TODO:
	}
};


///////////////////////////////////////////////////////////////////////////////////////
// INTERNAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////////
static mas_archtype_tracker g_tracker = { };


///////////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_archtype_init()
{
	if (!g_ents.init())
		return false;
	if (!g_tracker.create())
	{
		g_ents.deinit();
		return false;
	}
	
	return true;
}

void mas_archtype_deinit()
{
	g_tracker.destroy();
	g_ents.deinit();
}

mas_archtype* mas_archtype_find(const mas_component_query* comp_query)
{
	return g_tracker.find(comp_query);
}

mas_archtype* mas_archtype_create(const mas_component_query* comp_query) 
{
	return g_tracker.add(comp_query);
}

void mas_archtype_destroy(mas_archtype* archtype)
{
	// TODO:
}

mas_entity mas_archtype_new_entity(mas_archtype* archtype) 
{
	if (!archtype)
		return { 0 };
	mas_entity_handle ent_handle = archtype->create_entity();
	return { ent_handle.handle };
}

void mas_archtype_remove_entity(mas_entity entity)
{
	mas_entity_handle ent_handle = { entity.id };
	mas_entity_mapper* ent_mapper = g_ents.mappers.get_element(ent_handle.mapper_idx);
	if (!ent_mapper || (ent_mapper->gen != ent_handle.gen))
		return;

	mas_archtype* archtype = g_tracker.get_archtype(ent_mapper->archtype_idx);
	if (!archtype)
		return;
	return archtype->destroy_entity(ent_handle);
}
