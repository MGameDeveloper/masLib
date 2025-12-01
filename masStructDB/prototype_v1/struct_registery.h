#pragma once

#include <stdint.h>

typedef struct masMemberDesc
{
	const char* type;
	const char* name;
	uint32_t    size;
};
typedef struct masStructDesc
{
	const char* name;
	masMemberDesc* members;
	uint32_t       alignment;
	uint32_t       size;
	uint32_t       member_count;
};

#define mas_array_size(a) (sizeof(a)/sizeof(a[0]))
#define mas_member(t, n)\
    {#t, #n, sizeof(t)}    
#define mas_struct(t, ...)\
    masMemberDesc t##members_desc[] = { __VA_ARGS__ };\
    masStructDesc t##struct_desc    = { #t, t##members_desc, sizeof(t), mas_array_size(t##members_desc)};\
    mas_sttrct_registery_add(&t##struct_desc);

bool mas_struct_registery_init();
void mas_struct_registery_deinit();
void mas_sttrct_registery_add(masStructDesc* desc);