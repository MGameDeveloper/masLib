#pragma once

#include <stdint.h>


typedef struct mas_struct_member_desc
{
	const char *type;
	const char *name;
	uint32_t    size;
};

typedef struct mas_struct_desc
{
	mas_struct_member_desc *members;
	const char *name;
	uint32_t    alignment;
	uint32_t    size;
	uint32_t    member_count;
};

#define mas_array_size(a) (sizeof(a)/sizeof(a[0]))
#define mas_member(type, name)\
    {#type, #name, sizeof(type)}    
#define mas_struct(t, ...)\
    mas_struct_member_desc t##members_desc[] = { __VA_ARGS__ };\
    mas_struct_desc        t##struct_desc    = {t##members_desc, #t, alignof(t), sizeof(t), mas_array_size(t##members_desc)};\
    mas_struct_registery_add(&t##struct_desc);

bool mas_struct_registery_init();
void mas_struct_registery_deinit();
bool mas_struct_registery_add(mas_struct_desc* desc);

// only for debug/test the implementation
void mas_struct_registery_print();

bool mas_struct_find_structs(const char** names, uint32_t* out_ids, uint32_t count);