///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "mas_memory.h"


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#define MAS_DEFAULT_FRAME_MEMORY_SIZE     (1024llu * 16llu)
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))
#define MAS_MALLOC(type, size)            (type*)malloc(size)
#define MAS_FREE(ptr)                            free(ptr)


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
enum mas_memory_type
{
    mas_memory_type_unknown,
    mas_memory_type_page,
    mas_memory_type_array,
    mas_memory_type_stack,

    mas_memory_type_count,
};

union mas_memory_handle
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

//////////////////////////////////
// DATA STRUCTURE DEFEINITIONS
struct mas_memory_page 
{
    void     *data;
    uint32_t  size;
    uint16_t  gen;
};

struct mas_memory_array
{
    void     *array;
    uint32_t  element_size;
    uint32_t  capacity;
    uint32_t  count;
    uint16_t  gen;
};

struct mas_memory_stack
{
    void     *data;
    uint32_t  element_size;
    uint32_t  capacity;
    uint32_t  count;
    uint32_t  gen;
};

///////////////////////////////////
// LISTS DEFINITIONS
struct mas_memory_array_list
{
    mas_memory_array *list;
    int32_t          *free_indices;
    int32_t           free_count;
    int32_t           count;
    int32_t           capacity;
};

struct mas_memory_page_list
{
    mas_memory_page *list;
    int32_t         *free_indices;
    int32_t          free_count;
    int32_t          count;
    int32_t          capacity;
};

struct mas_memory_stack_list
{
    mas_memory_stack *list;
    int32_t          *free_indices;
    int32_t           free_count;
    int32_t           count;
    int32_t           capacity;
};

struct mas_memory_frame_scope
{
    uint8_t* data;
    uint32_t size;
    uint32_t alloc_idx;
};

////////////////////////////////////////////////
// ECS MEMORY OWNER
struct mas_memory
{
    mas_memory_frame_scope frame;
    mas_memory_page_list   pages;
    mas_memory_array_list  arrays;
    mas_memory_stack_list  stacks;
};


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static mas_memory* g_mem = NULL;


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
static bool mas_internal_resize_page_list()
{
    if (!g_mem)
        return false;
    
    mas_memory_page_list* pages_mem = &g_mem->pages;

    if (pages_mem->capacity == 0)
        pages_mem->capacity = 1;

    uint32_t new_capacity      = pages_mem->capacity         * 2;
    uint32_t page_entries_size = sizeof(mas_memory_page) * new_capacity;
    uint32_t free_indices_size = sizeof(int32_t)             * new_capacity;
    uint32_t mem_size          = page_entries_size + free_indices_size;

    void* mem = MAS_MALLOC(void, mem_size);
    if (!mem)
        return false;
    memset(mem, 0, mem_size);

    mas_memory_page *new_pages        = MAS_PTR_OFFSET(mas_memory_page, mem, 0);
    int32_t             *new_free_indices = MAS_PTR_OFFSET(int32_t,             mem, page_entries_size);

    if (pages_mem->list)
    {
        memcpy(new_pages,        pages_mem->list,         pages_mem->capacity * sizeof(mas_memory_page));
        memcpy(new_free_indices, pages_mem->free_indices, pages_mem->capacity * sizeof(int32_t));
        free(pages_mem->list);
        pages_mem->list         = NULL;
        pages_mem->free_indices = NULL;
    }

    pages_mem->list         = new_pages;
    pages_mem->free_indices = new_free_indices;
    pages_mem->capacity     = new_capacity;

    return true;
}

static bool mas_internal_resize_array_list()
{
    if (!g_mem)
        return false;

    mas_memory_array_list* arrays_mem = &g_mem->arrays;
    if (arrays_mem->capacity == 0)
        arrays_mem->capacity = 1;

    uint32_t new_capacity           = arrays_mem->capacity         * 2;
    uint32_t new_array_entries_size = sizeof(mas_memory_array) * new_capacity;
    uint32_t new_free_indices_size  = sizeof(int32_t)              * new_capacity;
    uint32_t mem_size = new_array_entries_size + new_free_indices_size;
    void* mem = MAS_MALLOC(void, mem_size);
    if (!mem)
        return false;
    memset(mem, 0, mem_size);

    mas_memory_array *new_array_entries    = MAS_PTR_OFFSET(mas_memory_array, mem, 0);
    int32_t              *new_free_indices = MAS_PTR_OFFSET(int32_t, mem, new_array_entries_size);

    if (arrays_mem->list)
    {
        memcpy(new_array_entries, arrays_mem->list, arrays_mem->capacity * sizeof(mas_memory_array));
        memcpy(new_free_indices, arrays_mem->free_indices, arrays_mem->capacity * sizeof(int32_t));
        free(arrays_mem->list);
        arrays_mem->list         = NULL;
        arrays_mem->free_indices = NULL;
    }

    arrays_mem->list         = new_array_entries;
    arrays_mem->free_indices = new_free_indices;
    arrays_mem->capacity     = new_capacity;

    return true;
}

static bool mas_internal_resize_stack_list()
{
    if (!g_mem)
        return false;

    mas_memory_stack_list* stacks = &g_mem->stacks;
    if (stacks->capacity == 0)
        stacks->capacity = 1;

    size_t new_capacity           = stacks->capacity             * 2;
    size_t new_stack_entries_size = sizeof(mas_memory_stack) * new_capacity;
    size_t new_free_indices_size  = sizeof(int32_t)              * new_capacity;
    size_t mem_size = new_stack_entries_size + new_free_indices_size;
    void* mem = MAS_MALLOC(void, mem_size);
    if (!mem)
        return false;
    memset(mem, 0, mem_size);

    mas_memory_stack *new_stack_entries = MAS_PTR_OFFSET(mas_memory_stack, mem, 0);
    int32_t              *new_free_indices  = MAS_PTR_OFFSET(int32_t,              mem, new_stack_entries_size);

    if (stacks->list)
    {
        memcpy(new_stack_entries, stacks->list,         sizeof(mas_memory_stack) * stacks->capacity);
        memcpy(new_free_indices,  stacks->free_indices, sizeof(int32_t)              * stacks->capacity);
        free(stacks->list);
        stacks->list         = NULL;
        stacks->free_indices = NULL;
    }
    stacks->list         = new_stack_entries;
    stacks->free_indices = new_free_indices;
    stacks->capacity     = new_capacity;

    return true;
}

static mas_memory_page* mas_internal_get_page(mas_memory_handle mem_handle)
{
    if (!g_mem || (mem_handle.type != mas_memory_type_page))
        return NULL;

    mas_memory_page_list* pages_mem = &g_mem->pages;
    if (!pages_mem->list)
        return NULL;

    if (mem_handle.gen == 0 || mem_handle.idx >= pages_mem->capacity)
        return NULL;

    mas_memory_page* page = &pages_mem->list[mem_handle.idx];
    if (page->gen != mem_handle.gen)
        return NULL;

    return page;
}

static mas_memory_array* mas_internal_get_array(mas_memory_handle mem_handle)
{
    if (!g_mem || (mem_handle.type != mas_memory_type_array))
        return NULL;

    mas_memory_array_list* arrays_mem = &g_mem->arrays;
    if (!arrays_mem->list)
        return NULL;

    if (mem_handle.gen == 0 || (mem_handle.idx >= arrays_mem->capacity))
        return NULL;

    mas_memory_array* array = &arrays_mem->list[mem_handle.idx];
    if (array->gen != mem_handle.gen)
        return NULL;

    return array;
}

static mas_memory_stack* mas_internal_get_stack(mas_memory_stack_id stack_id)
{
    if (!g_mem)
        return NULL;

    mas_memory_handle mem_handle = { stack_id.id };
    if ((mem_handle.type != mas_memory_type_stack) || !g_mem->stacks.list)
        return NULL;

    if (mem_handle.gen == 0 || (mem_handle.idx >= g_mem->stacks.capacity))
        return NULL;

    mas_memory_stack* stack = &g_mem->stacks.list[mem_handle.idx];
    if (stack->gen != mem_handle.gen)
        return NULL;

    return stack;
}


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_memory_init()
{
    if(g_mem)
        return true;

    g_mem = (mas_memory*)malloc(sizeof(mas_memory));
    if(!g_mem)
        return false;
    memset(g_mem, 0, sizeof(mas_memory));

    // initialize frame scope memory
    g_mem->frame.size      = MAS_DEFAULT_FRAME_MEMORY_SIZE;
    g_mem->frame.alloc_idx = 0;
    g_mem->frame.data      = (uint8_t*)malloc(MAS_DEFAULT_FRAME_MEMORY_SIZE);
    if (!g_mem->frame.data)
    {
        mas_memory_deinit();
        return false;
    }

    return true;
}

void mas_memory_deinit()
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

        memset(g_mem, 0, sizeof(mas_memory));
    }
}

void mas_memory_zero(void* data, size_t size)
{
    if (!data || size == 0)
        return;
    memset(data, 0, size);
}

// FRAME SCOPE ALLOCATION API
void* mas_memory_frame_malloc(size_t size)
{
    if (!g_mem)
        return NULL;

    mas_memory_frame_scope* frame_mem = &g_mem->frame;
    if((frame_mem->alloc_idx + size) > frame_mem->size)
        return NULL;

    void* data = &frame_mem->data[frame_mem->alloc_idx];
    frame_mem->alloc_idx += size;

    return data;
}

void  mas_memory_frame_reset()
{
    if(g_mem)
    {
        mas_memory_frame_scope* frame_mem = &g_mem->frame;

        memset(frame_mem->data, 0, frame_mem->size);
        frame_mem->alloc_idx = 0;
    }
}


// PAGE ALLOCATION API
mas_memory_page_id mas_memory_page_create()
{
    mas_memory_handle mem_handle = { 0 };

    if(!g_mem)
        return { 0 };

    mas_memory_page_list* pages_mem = &g_mem->pages;

    int32_t page_idx = -1;
    if (pages_mem->count + 1 >= pages_mem->capacity)
    {
        if (pages_mem->free_count <= 0)
        {
            if (!mas_internal_resize_page_list())
                return { 0 };
            else
                page_idx = pages_mem->count++;
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

    mas_memory_page* page = &pages_mem->list[page_idx];
    if (page->gen == 0)
        page->gen = 1;
    mem_handle.idx  = page_idx;
    mem_handle.gen  = page->gen;
    mem_handle.type = mas_memory_type_page;

    return { mem_handle.handle };
}

void  mas_memory_page_free(mas_memory_page_id page_id)
{
    mas_memory_handle mem_handle = { page_id.id };
    mas_memory_page* page = mas_internal_get_page(mem_handle);
    if (!page)
        return;

    page->gen++;
    if(page->gen == 0)
        page->gen = 1;

    memset(page->data, 0, page->size);

   g_mem->pages.free_indices[g_mem->pages.free_count++] = mem_handle.idx;
}

void* mas_memory_page_data(mas_memory_page_id page_id)
{
    mas_memory_handle mem_handle = { page_id.id };
    mas_memory_page* page = mas_internal_get_page(mem_handle);
    if (!page)
        return NULL;
    return page->data;
}

size_t mas_memory_page_size(mas_memory_page_id page_id)
{
    mas_memory_handle mem_handle = { page_id.id };
    mas_memory_page* page = mas_internal_get_page(mem_handle);
    if (!page)
        return NULL;
    return page->size;
}

bool mas_memory_page_is_valid(mas_memory_page_id page_id)
{
    mas_memory_handle  page_handle = { page_id.id };
    mas_memory_page   *page        = mas_internal_get_page(page_handle);
    if (!page)
        return false;
    return true;
}


// ARRAY ALLOCATION API
mas_memory_array_id mas_memory_array_create(size_t element_size)
{
    mas_memory_handle mem_handle = { 0 };

    if (!g_mem)
        return { 0 };

    mas_memory_array_list* arrays_mem = &g_mem->arrays;
    if (!arrays_mem->list)
        return { 0 };

    int32_t array_idx = -1;
    if (arrays_mem->count + 1 >= arrays_mem->capacity)
    {
        if (arrays_mem->free_count <= 0)
        {
            if (!mas_internal_resize_array_list())
                return { 0 };
            else
                array_idx = arrays_mem->count++;
        }
        else
        {
            array_idx = arrays_mem->free_indices[--arrays_mem->free_count];
            arrays_mem->free_indices[arrays_mem->free_count] = -1;

            // EXPERMENT:
            // reclaculate the capacity if previouse array has 4 elements with every element being 32 byte 
            //   and now the array is used for an element of 4 byte -> ( 4 * 32 ) / 4 = new capacity
            //   this would tread the current array from beign 4 element of 32 byte as 32 element of 4 byte
            //   without additional allocation just using previous freed arrays
            mas_memory_array* local_array = &arrays_mem->list[array_idx];
            size_t total_byte   = local_array->element_size * local_array->capacity;
            size_t new_capacity = total_byte / element_size;
            if (new_capacity == 0)
            {
                // would left it to the add function to resize it
            }
            else
                local_array->capacity = new_capacity;
        }
    }
    else
    {
        array_idx = arrays_mem->count++;
    }

    if (array_idx == -1)
        return { 0 };

    mas_memory_array* array = &arrays_mem->list[array_idx];
    array->element_size = element_size;
    array->count        = 0;
    if (array->gen == 0)
        array->gen = 1;
    mem_handle.idx  = array_idx;
    mem_handle.gen  = array->gen;
    mem_handle.type = mas_memory_type_array;
    
    return { mem_handle.handle };
}

void mas_memory_array_free(mas_memory_array_id array_id)
{
    mas_memory_handle mem_handle = { array_id.id };  
    mas_memory_array* array = mas_internal_get_array(mem_handle);
    if (!array)
        return;

    array->gen++;
    if (array->gen == 0)
        array->gen = 1;
    memset(array->array, 0, array->element_size * array->capacity);

    g_mem->arrays.free_indices[g_mem->arrays.free_count++] = mem_handle.idx;
}

void mas_memory_array_clear(mas_memory_array_id array_id)
{
    mas_memory_handle mem_handle = { array_id.id };
    mas_memory_array* array = mas_internal_get_array(mem_handle);
    memset(array->array, 0, array->element_size * array->capacity);
}

void* mas_memory_array_new_element(mas_memory_array_id array_id)
{
    mas_memory_handle mem_handle = { array_id.id };
    mas_memory_array* array = mas_internal_get_array(mem_handle);
    if (!array)
        return NULL;
   
    if ((array->count + 1) >= array->capacity)
    {
        if (array->capacity == 0)
            array->capacity = 1;

        uint32_t  new_capacity   = array->capacity * 2;
        uint32_t  new_array_size = new_capacity * array->element_size;
        void     *new_array      = MAS_MALLOC(void, new_array_size);
        if (!new_array)
            return NULL;
        memset(new_array, 0, new_array_size);

        if (array->array)
        {
            memcpy(new_array, array->array, array->element_size * array->capacity);
            free(array->array);
            array->array = NULL;
        }

        array->array    = new_array;
        array->capacity = new_capacity;
    }

    uint32_t element_idx = array->count++;
    void* element = MAS_PTR_OFFSET(void, array->array, array->element_size * element_idx);
    return element;
}

void* mas_memory_array_get_element(mas_memory_array_id array_id, size_t idx)
{
    mas_memory_handle mem_handle = { array_id.id };
    mas_memory_array* array = mas_internal_get_array(mem_handle);
    if (!array || (idx >= array->capacity))
        return NULL;

    void* element = MAS_PTR_OFFSET(void, array->array, array->element_size * idx);
    return element;
}

size_t mas_memory_array_element_count(mas_memory_array_id array_id)
{
    mas_memory_handle mem_handle = { array_id.id };
    mas_memory_array* array = mas_internal_get_array(mem_handle);
    if (!array)
        return 0;
    return array->count;
}

size_t mas_memory_array_capacity(mas_memory_array_id array_id)
{
    mas_memory_handle array_handle = { array_id.id };
    mas_memory_array* array = mas_internal_get_array(array_handle);
    if (!array)
        return 0;
    return array->capacity;
}

bool mas_memory_array_resize(mas_memory_array_id array_id)
{
    mas_memory_handle  array_handle = { array_id.id };
    mas_memory_array  *array        = mas_internal_get_array(array_handle);
    if (!array)
        return false;

    if (array->capacity == 0)
        array->capacity = 1;

    uint32_t  new_capacity   = array->capacity * 2;
    uint32_t  new_array_size = new_capacity * array->element_size;
    void* new_array = MAS_MALLOC(void, new_array_size);
    if (!new_array)
        return false;
    memset(new_array, 0, new_array_size);

    if (array->array)
    {
        memcpy(new_array, array->array, array->element_size * array->capacity);
        free(array->array);
        array->array = NULL;
    }

    array->array    = new_array;
    array->capacity = new_capacity;

    return true;
}

bool mas_memory_array_is_valid(mas_memory_array_id array_id)
{
    mas_memory_handle  array_handle = { array_id.id };
    mas_memory_array  *array        = mas_internal_get_array(array_handle);
    if (!array)
        return false;
    return true;
}


// STACK ALLOCATION API
mas_memory_stack_id mas_memory_stack_create(size_t element_size)
{
    if (!g_mem)
        return { 0 };

    mas_memory_stack_list* stacks = &g_mem->stacks;
    if (!stacks->list)
        return { 0 };

    int32_t stack_idx = -1;
    if (stacks->count + 1 >= stacks->capacity)
    {
        if (stacks->free_count <= 0)
        {
            if (!mas_internal_resize_stack_list())
                return { 0 };
            else
                stack_idx = stacks->count++;
        }
        else
        {
            stack_idx = stacks->free_indices[--stacks->free_count];
            stacks->free_indices[stacks->free_count] = -1;

            mas_memory_stack* local_stack = &stacks->list[stack_idx];
            size_t total_byte = local_stack->element_size * local_stack->capacity;
            size_t new_capacity = total_byte / local_stack->element_size;
            if (new_capacity == 0)
            {
                // would left it to push function to resize it
            }
            else
                local_stack->capacity = new_capacity;
        }
    }
    else
    {
        stack_idx = stacks->count++;
    }

    if (stack_idx == -1)
        return { 0 };

    mas_memory_stack* stack = &stacks->list[stack_idx];
    if (stack->gen == 0)
        stack->gen = 1;
    stack->element_size = element_size;
    stack->count = 0;

    mas_memory_handle mem_handle = { 0 };
    mem_handle.idx = stack_idx;
    mem_handle.gen = stack->gen;
    mem_handle.type = mas_memory_type_stack;
    
    return { mem_handle.handle };
}

void mas_memory_stack_free(mas_memory_stack_id stack_id)
{
    mas_memory_stack* stack = mas_internal_get_stack(stack_id);
    if (!stack)
        return;

    stack->gen++;
    if (stack->gen == 0)
        stack->gen = 1;
    memset(stack->data, 0, stack->element_size * stack->capacity);

    mas_memory_handle mem_handle = { stack_id.id };
    g_mem->stacks.free_indices[g_mem->stacks.free_count++] = mem_handle.idx;
}

void* mas_memory_stack_top_element(mas_memory_stack_id stack_id)
{
    mas_memory_stack* stack = mas_internal_get_stack(stack_id);
    if (!stack)
        return NULL;
    if (stack->count == 0)
        return NULL;
    void* element = MAS_PTR_OFFSET(void, stack->data, stack->element_size * (stack->count - 1));
    return element;
}

void mas_memory_stack_pop_element(mas_memory_stack_id stack_id)
{
    mas_memory_stack* stack = mas_internal_get_stack(stack_id);
    if (!stack)
        return;
    if (stack->count == 0)
        return;
    stack->count--;
    void* element = MAS_PTR_OFFSET(void, stack->data, stack->element_size * stack->count);
    memset(element, 0, stack->element_size);
}

void* mas_memory_stack_push_element(mas_memory_stack_id stack_id)
{
    mas_memory_stack* stack = mas_internal_get_stack(stack_id);
    if (!stack)
        return NULL;

    int32_t element_idx = -1;
    if ((stack->count + 1) >= stack->capacity)
    {
        if (stack->capacity == 0)
            stack->capacity = 1;

        size_t  new_capacity = stack->capacity * 2;
        size_t  stack_size   = stack->element_size * new_capacity;
        void   *stack_mem    = MAS_MALLOC(void, stack_size);
        if (!stack_mem)
            return NULL;
        memset(stack_mem, 0, stack_size);

        if (stack->data)
        {
            memcpy(stack_mem, stack->data, stack->element_size * stack->element_size);
            free(stack->data);
            stack->data = NULL;
        }

        stack->data     = stack_mem;
        stack->capacity = new_capacity;
    }

    uint32_t element_idx = stack->count++;
    void* element = MAS_PTR_OFFSET(void, stack->data, stack->element_size * element_idx);
    return element;
}

bool mas_memory_stack_is_empty(mas_memory_stack_id stack_id)
{
    mas_memory_stack* stack = mas_internal_get_stack(stack_id);
    if (!stack)
        return false;
    if (stack->count > 0)
        return false;
    return true;
}

bool mas_memory_stack_is_valid(mas_memory_stack_id stack_id)
{
    mas_memory_stack* stack = mas_internal_get_stack(stack_id);
    if (!stack)
        return false;
    return true;
}