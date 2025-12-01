#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>

#include "mas_mmap.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_VIEW_SIZE 1024
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERNAL
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool mas_internal_is_mmap_valid(mas_mmap_t* file)
{
    if (!file || !file->handle || !file->map_handle || !file->view_base)
        return false;
    return true;
}

bool mas_internal_resize_mmap_view(mas_mmap_t* file)
{
    LARGE_INTEGER file_size = {};
    file_size.QuadPart = file->view_size + MAS_VIEW_SIZE;

    if (UnmapViewOfFile(file->view_base))
    {
        CloseHandle(file->map_handle);

        if (!SetFilePointerEx(file->handle, file_size, NULL, FILE_BEGIN))
        {
            // log error
            return false;
        }

        if (!SetEndOfFile(file->handle))
        {
            // log error
            return false;
        }

        file->view_size = file_size.QuadPart;

        file->map_handle = CreateFileMappingA(file->handle, NULL, PAGE_READWRITE, file_size.HighPart, file_size.LowPart, NULL);
        if (file->map_handle == INVALID_HANDLE_VALUE)
        {
            // log error
            return false;
        }

        file->view_base = MapViewOfFile(file->map_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (!file->view_base)
        {
            // log error
            return false;
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION MUST GO INTO .C OR .CPP
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  mas_mmap(mas_mmap_t* file, const char* path)
{
    if (!mas_internal_is_mmap_valid(file))
        return false;

    // check file if already created
    bool file_found = false;
    DWORD file_attributes = GetFileAttributesA(path);
    if (file_attributes != INVALID_FILE_ATTRIBUTES)
        file_found = true;

    // this would open or create if not existed
    HANDLE file_handle = CreateFileA(path, GENERIC_READ || GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return false;

    // extract size or set if not found
    LARGE_INTEGER file_size = { };
    if (!file_found)
    {
        file_size.QuadPart = MAS_VIEW_SIZE;
        if (!SetFilePointerEx(file_handle, file_size, NULL, FILE_BEGIN))
        {
            CloseHandle(file_handle);
            return false;
        }

        if (!SetEndOfFile(file_handle))
        {
            CloseHandle(file_handle);
            return false;
        }
    }
    else
    {
        if (!GetFileSizeEx(file_handle, &file_size))
        {
            CloseHandle(file_handle);
            return false;
        }
    }

    HANDLE file_map_handle = CreateFileMappingA(file_handle, NULL, PAGE_READWRITE, file_size.HighPart, file_size.LowPart, NULL);
    if (file_map_handle == INVALID_HANDLE_VALUE)
    {
        CloseHandle(file_handle);
        return false;
    }

    void* file_view_base = MapViewOfFile(file_map_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!file_view_base)
    {
        CloseHandle(file_map_handle);
        CloseHandle(file_handle);
        return false;
    }

    file->handle = file_handle;
    file->map_handle = file_map_handle;
    file->view_base = file_view_base;
    file->view_size = file_size.QuadPart;

    return true;
}

void mas_unmmap(mas_mmap_t* file)
{
    if (!mas_internal_is_mmap_valid(file))
        return;

    if (UnmapViewOfFile(file->view_base))
    {
        CloseHandle(file->map_handle);
        CloseHandle(file);
        memset(file, 0, sizeof(mas_mmap_t));
    }
    else
    {
        // log erro
    }

}

bool mas_mmap_write(mas_mmap_t* file, const void* data, size_t size)
{
    if (!mas_internal_is_mmap_valid(file) || !data || size == 0)
        return false;

    void* write_boundry = MAS_PTR_OFFSET(void, file->view_base, file->view_size);
    void* write_request = MAS_PTR_OFFSET(void, file->view_base, file->write_offset + size);
    if (write_request > write_boundry)
    {
        if (!mas_internal_resize_mmap_view(file))
        {
            // log error
            return false;
        }
    }

    void* write_location = MAS_PTR_OFFSET(void, file->view_base, file->write_offset);
    memcpy(write_location, data, size);
    file->write_offset += size;

    return true;
}

void* mas_mmap_read(mas_mmap_t* file, size_t offset, size_t size)
{
    if (!mas_internal_is_mmap_valid(file))
        return NULL;

    void* read_boundry = MAS_PTR_OFFSET(void, file->view_base, file->view_size);
    void* read_request = MAS_PTR_OFFSET(void, file->view_base, offset + size);
    if (read_request > read_boundry)
    {
        // log error
        return NULL;
    }

    void* data = MAS_PTR_OFFSET(void, file->view_base, offset);
    return data;
}

bool mas_mmap_is_valid(mas_mmap_t* file)
{
    if (!file || !mas_internal_is_mmap_valid(file))
        return false;
    return true;
}