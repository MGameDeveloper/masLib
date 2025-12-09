#pragma once

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#define MAS_DEFINE_MEMORY_HANDLE(n) struct n { uint64_t id; }

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
MAS_DEFINE_MEMORY_HANDLE(mas_memory_page_id);
MAS_DEFINE_MEMORY_HANDLE(mas_memory_array_id);
MAS_DEFINE_MEMORY_HANDLE(mas_memory_stack_id);


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
bool mas_memory_init();
void mas_memory_deinit();

void mas_memory_zero(void* data, size_t size);
void mas_memory_copy(void* dst, const void* src, size_t size);
size_t mas_memory_default_page_size();

// FRAME SCOPE ALLOCATION API
void* mas_memory_frame_malloc(size_t size);
void  mas_memory_frame_reset();

// PAGE ALLOCATION API CURRENTRLY EACH PAGE IS 16 KB
mas_memory_page_id mas_memory_page_create();
void               mas_memory_page_free(mas_memory_page_id page_id);
void*              mas_memory_page_data(mas_memory_page_id page_id);
size_t             mas_memory_page_size(mas_memory_page_id page_id);
bool               mas_memory_page_is_valid(mas_memory_page_id page_id);

// ARRAY ALLOCATION API
mas_memory_array_id mas_memory_array_create(size_t element_size);
void                mas_memory_array_free(mas_memory_array_id array_id);
void                mas_memory_array_clear(mas_memory_array_id array_id);
void*               mas_memory_array_new_element(mas_memory_array_id);
void*               mas_memory_array_get_element(mas_memory_array_id, size_t idx);
size_t              mas_memory_array_element_count(mas_memory_array_id array_id);
size_t              mas_memory_array_capacity(mas_memory_array_id array_id);
bool                mas_memory_array_reserve(mas_memory_array_id array_id, size_t capacity);
bool                mas_memory_array_is_valid(mas_memory_array_id array_id);

// STACK ALLOCATION API
mas_memory_stack_id mas_memory_stack_create(size_t element_size);
void                mas_memory_stack_free(mas_memory_stack_id stack_id);
void*               mas_memory_stack_top_element(mas_memory_stack_id stack_id);
void                mas_memory_stack_pop_element(mas_memory_stack_id stack_id);
void*               mas_memory_stack_push_element(mas_memory_stack_id stack_id);
bool                mas_memory_stack_is_empty(mas_memory_stack_id stack_id);
bool                mas_memory_stack_is_valid(mas_memory_stack_id stack_id);


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
#define MAS_FRAME_MEMORY_MALLOC(type, size) (type*)mas_memory_frame_malloc(size)

//////////////////////////////////////////////////////////





