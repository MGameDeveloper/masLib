#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "file_mapper.h"



bool mas_mmap(mas_mmap_t* map, const char* path)
{
	if (!map || map->file)
		return false;

	HANDLE file = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;

	HANDLE file_map = CreateFileMappingA(file, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (file_map == INVALID_HANDLE_VALUE)
	{
		CloseHandle(file);
		return false;
	}

	void* file_view = MapViewOfFile(file_map, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!file_view)
	{
		CloseHandle(file_map);
		CloseHandle(file);
		return false;
	}
}

void mas_unmmap(mas_mmap_t* map)
{

}

bool mas_mmap_write(mas_mmap_t* map, const void* data, size_t size)
{

}