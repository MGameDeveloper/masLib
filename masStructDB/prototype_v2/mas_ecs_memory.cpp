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
#define MAS_DEFAULT_PAGE_COUNT 256
#define MAS_DEFAULT_PAGE_SIZE         (1024llu * 16llu)
#define MAS_DEFAULT_FRAME_MEMORY_SIZE (1024llu * 16llu)

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
union mas_ecs_memory_handle
{
    mas_ecs_memory_t handle;
    struct
    {
        uint16_t idx;
        uint16_t gen;
    };
};

struct mas_ecs_memory_page 
{
    void     *data;
    uint32_t  size;
    uint16_t  gen;
};

struct mas_ecs_memory
{
    mas_ecs_memory_page   pages[MAS_DEFAULT_PAGE_COUNT];
    int32_t               free_page_indices[MAS_DEFAULT_PAGE_COUNT];
    int32_t               page_idx;
    int32_t               free_page_count;
    int32_t               frame_malloc_idx;
    uint8_t               frame[MAS_DEFAULT_FRAME_MEMORY_SIZE];
};


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static mas_ecs_memory* g_ecs_mem = NULL;


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool  mas_ecs_memory_init()
{
    if(g_ecs_mem)
        return true;

    g_ecs_mem = (mas_ecs_memory*)malloc(sizeof(mas_ecs_memory));
    if(!g_ecs_mem)
        return false;
    memset(g_ecs_mem, 0, sizeof(mas_ecs_memory));
    return true;
}

void mas_ecs_memory_deinit()
{
    if(g_ecs_mem)
    {
        free(g_ecs_mem);
        g_ecs_mem = NULL;
    }
}

void* mas_ecs_memory_frame_malloc(size_t size)
{
    if(!g_ecs_mem || (g_ecs_mem->frame_malloc_idx + size) > MAS_DEFAULT_FRAME_MEMORY_SIZE)
        return NULL;

    void* data = &g_ecs_mem->frame[g_ecs_mem->frame_malloc_idx];
    g_ecs_mem->frame_malloc_idx += size;

    return data;
}

void  mas_ecs_memory_frame_reset()
{
    if(g_ecs_mem)
    {
        memset(g_ecs_mem->frame, 0, MAS_DEFAULT_FRAME_MEMORY_SIZE);
        g_ecs_mem->frame_malloc_idx = 0;
    }
}

mas_ecs_memory_t mas_ecs_memory_malloc_page()
{
    mas_ecs_memory_handle mem_handle = { 0 };
    if(!g_ecs_mem)
        return mem_handle.handle;

    int32_t page_idx = -1;
    mas_ecs_memory_page* page = NULL;
    if(g_ecs_mem->page_idx < MAS_DEFAULT_PAGE_COUNT)
    {
        page_idx = g_ecs_mem->page_idx++;
        page     = &g_ecs_mem->pages[page_idx];

        page       = &g_ecs_mem->pages[page_idx];
        page->data = malloc(MAS_DEFAULT_PAGE_SIZE);
        page->size = MAS_DEFAULT_PAGE_SIZE;
    }
    else
    {
        if(g_ecs_mem->free_page_count > 0)
        {
            page_idx = g_ecs_mem->free_page_indices[--g_ecs_mem->free_page_count];
            page     = &g_ecs_mem->pages[page_idx];
        }
    }

    if(page)
    {
        if(page->gen == 0)
            page->gen = 1;
        mem_handle.idx = page_idx;
        mem_handle.gen = page->gen;
    }

    return mem_handle.handle;
}

void  mas_ecs_memory_free_page(mas_ecs_memory_t page_id)
{
    mas_ecs_memory_handle mem_handle = { page_id };

    if(!g_ecs_mem || mem_handle.gen == 0 || mem_handle.idx >= MAS_DEFAULT_PAGE_COUNT)
        return;

    mas_ecs_memory_page* page = &g_ecs_mem->pages[mem_handle.idx];
    if(page->gen != mem_handle.gen)
        return;

    page->gen++;
    if(page->gen == 0)
        page->gen = 1;

    memset(page->data, 0, page->size);

    g_ecs_mem->free_page_indices[g_ecs_mem->free_page_count++] = mem_handle.idx;
}

void* mas_ecs_memory_page_data(mas_ecs_memory_t page_id)
{
    mas_ecs_memory_handle mem_handle = { page_id };

    if(!g_ecs_mem || mem_handle.gen == 0 || mem_handle.idx >= MAS_DEFAULT_PAGE_COUNT)
        return NULL;

    mas_ecs_memory_page* page = &g_ecs_mem->pages[mem_handle.idx];
    if(page->gen != mem_handle.gen)
        return NULL;

    return page->data;
}

size_t mas_ecs_memory_page_size(mas_ecs_memory_t page_id)
{
    mas_ecs_memory_handle mem_handle = { page_id };

    if(!g_ecs_mem || mem_handle.gen == 0 || mem_handle.idx >= MAS_DEFAULT_PAGE_COUNT)
        return 0;

    mas_ecs_memory_page* page = &g_ecs_mem->pages[mem_handle.idx];
    if(page->gen != mem_handle.gen)
        return NULL;

    return page->size;
}