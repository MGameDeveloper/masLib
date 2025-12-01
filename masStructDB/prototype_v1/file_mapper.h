#pragma once

#include <stdint.h>

typedef struct
{
	HANDLE   file;
	HANDLE   map;
	void*    view;
	uint64_t view_base;
	uint64_t view_size;
}mas_mmap_t;

bool mas_mmap(mas_mmap_t* map, const char* path);
void mas_unmmap(mas_mmap_t* map);
bool mas_mmap_write(mas_mmap_t* map, const void* data, size_t size);