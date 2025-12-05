///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "mas_ecs_memory.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#define MAS_DEFAULT_FRAME_MEMORY_SIZE     (1024llu * 16llu)
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_MALLOC(type, size)            (type*)malloc(size)
#define MAS_FREE(ptr)                            free(ptr)


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
enum mas_ecs_memory_type
{
    mas_ecs_memory_type_unknown,
    mas_ecs_memory_type_page,
    mas_ecs_memory_type_array,

};

union mas_ecs_memory_handle
{
    uint64_t handle;
    struct
    {
        uint16_t idx;
        uint16_t gen;
        uint8_t  type;
        uint8_t  _Pad[3];
    };
};

struct mas_ecs_memory_page 
{
    void     *data;
    uint32_t  size;
    uint16_t  gen;
};

struct mas_ecs_memory_array
{
    void     *array;
    uint32_t  element_size;
    uint32_t  capacity;
    uint32_t  count;
    uint16_t  gen;
};

struct mas_ecs_memory_array_list
{
    mas_ecs_memory_array *list;
    int32_t              *free_indices;
    int32_t               free_count;
    int32_t               count;
    int32_t               capacity;
};

struct mas_ecs_memory_page_list
{
    mas_ecs_memory_page *list;
    int32_t             *free_indices;
    int32_t              free_count;
    int32_t              count;
    int32_t              capacity;
};

struct mas_ecs_memory_frame_scope
{
    uint8_t* data;
    uint32_t size;
    uint32_t alloc_idx;
};

struct mas_ecs_memory
{
    mas_ecs_memory_frame_scope frame;
    mas_ecs_memory_page_list   pages;
    mas_ecs_memory_array_list  arrays;
};


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static mas_ecs_memory* g_mem = NULL;


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static bool mas_internal_resize_page_list()
{
    if (!g_mem)
        return false;
    
    mas_ecs_memory_page_list* pages_mem = &g_mem->pages;
    if (!pages_mem->list)
    {
        uint32_t  new_capacity      = 2;
        uint32_t  pages_entry_size  = sizeof(mas_ecs_memory_page) * new_capacity;
        uint32_t  free_indices_size = sizeof(int32_t)             * new_capacity;
        uint64_t  mem_size          = (pages_entry_size + free_indices_size);
        void     *mem               = MAS_MALLOC(void, mem_size);
        if (!mem)
            return false;

        pages_mem->list         = MAS_PTR_OFFSET(mas_ecs_memory_page, mem, 0);
        pages_mem->free_indices = MAS_PTR_OFFSET(int32_t,             mem, pages_entry_size);
        pages_mem->capacity     = new_capacity;
        pages_mem->count        = 0;
        pages_mem->free_count   = 0;
    }
    else
    {
        // allocate new memory
        uint32_t  new_capacity      = pages_mem->capacity * 2;
        uint32_t  pages_entry_size  = sizeof(mas_ecs_memory_page) * new_capacity;
        uint32_t  free_indices_size = sizeof(int32_t)             * new_capacity;
        uint64_t  mem_size          = (pages_entry_size + free_indices_size);
        void     *mem               = MAS_MALLOC(void, mem_size);
        if (!mem)
            return false;

        // Copy old data into the new allocated memory
        uint32_t             old_pages_entry_size  = sizeof(mas_ecs_memory_page) * pages_mem->capacity;
        uint32_t             old_free_indices_size = sizeof(int32_t) * pages_mem->capacity;
        mas_ecs_memory_page *new_list              = MAS_PTR_OFFSET(mas_ecs_memory_page, mem, 0);
        int32_t             *new_free_indices      = MAS_PTR_OFFSET(int32_t, mem, pages_entry_size);
        memcpy(new_list,         pages_mem->list,         old_pages_entry_size);
        memcpy(new_free_indices, pages_mem->free_indices, old_free_indices_size);

        // free old memory and point it to the new allocated memory
        free(pages_mem->list);
        pages_mem->list         = new_list;
        pages_mem->free_indices = new_free_indices;
        pages_mem->capacity     = new_capacity;

        // TODO: check after resizing that page entries still valid and point to their data correctly
    }

    return true;
}

static bool mas_internal_resize_array_list()
{

}


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_ecs_memory_init()
{
    if(g_mem)
        return true;

    g_mem = (mas_ecs_memory*)malloc(sizeof(mas_ecs_memory));
    if(!g_mem)
        return false;
    memset(g_mem, 0, sizeof(mas_ecs_memory));

    // initialize frame scope memory
    g_mem->frame.size      = MAS_DEFAULT_FRAME_MEMORY_SIZE;
    g_mem->frame.alloc_idx = 0;
    g_mem->frame.data      = (uint8_t*)malloc(MAS_DEFAULT_FRAME_MEMORY_SIZE);
    if (!g_mem->frame.data)
    {
        mas_ecs_memory_deinit();
        return false;
    }

    return true;
}

void mas_ecs_memory_deinit()
{
    if(g_mem)
    {
        // free frame scope memory page
        if (g_mem->frame.data)
            free(g_mem->frame.data);

        // free all arrays
        if (g_mem->arrays.list)
        {
            for (int32_t i = 0; i < g_mem->arrays.capacity; ++i)
                if (g_mem->arrays.list[i].array)
                    free(g_mem->arrays.list[i].array);
            free(g_mem->arrays.list);
        }

        // free all pages
        if (g_mem->pages.list)
        {
            for (int32_t i = 0; i < g_mem->pages.capacity; ++i)
                if (g_mem->pages.list[i].data)
                    free(g_mem->pages.list[i].data);
            free(g_mem->pages.list);
        }

        memset(g_mem, 0, sizeof(mas_ecs_memory));
    }
}

// FRAME SCOPE ALLOCATION API
void* mas_ecs_memory_frame_malloc(size_t size)
{
    if (!g_mem)
        return NULL;

    mas_ecs_memory_frame_scope* frame_mem = &g_mem->frame;
    if((frame_mem->alloc_idx + size) > frame_mem->size)
        return NULL;

    void* data = &frame_mem->data[frame_mem->alloc_idx];
    frame_mem->alloc_idx += size;

    return data;
}

void  mas_ecs_memory_frame_reset()
{
    if(g_mem)
    {
        mas_ecs_memory_frame_scope* frame_mem = &g_mem->frame;

        memset(frame_mem->data, 0, frame_mem->size);
        frame_mem->alloc_idx = 0;
    }
}

// PAGE ALLOCATION API
mas_ecs_memory_page_id mas_ecs_memory_page_create()
{
    mas_ecs_memory_handle mem_handle = { 0 };

    if(!g_mem)
        return { 0 };

    mas_ecs_memory_page_list* pages_mem = &g_mem->pages;

    int32_t page_idx = -1;
    if (pages_mem->count + 1 >= pages_mem->capacity)
    {
        if (pages_mem->free_count <= 0)
        {
            if (!mas_internal_resize_page_list())
                return { 0 };
        }
        else
        {
            page_idx = pages_mem->free_indices[--pages_mem->free_count];
            pages_mem->free_indices[pages_mem->free_count] = -1;
        }
    }
    else
    {
        page_idx = pages_mem->count++;
    }

    if (page_idx == -1)
        return { 0 };

    mas_ecs_memory_page* page = &pages_mem->list[page_idx];
    if(page)
    {
        if(page->gen == 0)
            page->gen = 1;
        mem_handle.idx  = page_idx;
        mem_handle.gen  = page->gen;
        mem_handle.type = mas_ecs_memory_type_page;
    }

    return { mem_handle.handle };
}

void  mas_ecs_memory_page_free(mas_ecs_memory_page_id page_id)
{
    mas_ecs_memory_handle mem_handle = { page_id.id };

    if (!g_mem || (mem_handle.type != mas_ecs_memory_type_page))
        return;

    mas_ecs_memory_page_list* pages_mem = &g_mem->pages;
    if (!pages_mem->list)
        return;

    if (mem_handle.gen == 0 || mem_handle.idx >= pages_mem->capacity)
        return;

    mas_ecs_memory_page* page = &pages_mem->list[mem_handle.idx];
    if(page->gen != mem_handle.gen)
        return;

    page->gen++;
    if(page->gen == 0)
        page->gen = 1;

    memset(page->data, 0, page->size);

    pages_mem->free_indices[pages_mem->free_count++] = mem_handle.idx;
}

void* mas_ecs_memory_page_data(mas_ecs_memory_page_id page_id)
{
    mas_ecs_memory_handle mem_handle = { page_id.id };

    if (!g_mem || (mem_handle.type != mas_ecs_memory_type_page))
        return NULL;

    mas_ecs_memory_page_list* pages_mem = &g_mem->pages;
    if (!pages_mem->list)
        return NULL;

    if (mem_handle.gen == 0 || mem_handle.idx >= pages_mem->capacity)
        return NULL;

    mas_ecs_memory_page* page = &pages_mem->list[mem_handle.idx];
    if (page->gen != mem_handle.gen)
        return NULL;

    return page->data;
}

size_t mas_ecs_memory_page_size(mas_ecs_memory_page_id page_id)
{
    mas_ecs_memory_handle mem_handle = { page_id.id };

    if (!g_mem || (mem_handle.type != mas_ecs_memory_type_page))
        return NULL;

    mas_ecs_memory_page_list* pages_mem = &g_mem->pages;
    if (!pages_mem->list)
        return NULL;

    if (mem_handle.gen == 0 || mem_handle.idx >= pages_mem->capacity)
        return NULL;

    mas_ecs_memory_page* page = &pages_mem->list[mem_handle.idx];
    if (page->gen != mem_handle.gen)
        return NULL;

    return page->size;
}


// ARRAY ALLOCATION API
