#pragma once

#include "../mas_memory.h"

template<typename T>
class mas_page
{
private:
	mas_memory_page_id page;

public:
	mas_page(const mas_page&) = delete;
	mas_page(mas_page&&) = delete;
	mas_page& operator=(const mas_page&) = delete;
	mas_page& operator=(mas_page&) = delete;

public:
	mas_page() : page(0)
	{
	}

	~mas_page()
	{

	}

	bool create()
	{
		if (page.id == 0)
			page = mas_memory_page_create();
		return (page.id != 0);
	}

	T*     data()    { return (T*)mas_memory_page_data(page); }
	size_t size()    { return mas_memory_page_size(page);     }
	void   destroy() { mas_memory_page_free(page);            }
	bool is_valid()  { return mas_memory_page_is_valid(page); }
};