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
		destroy();
	}

	bool create()
	{
		if (page == 0)
			page = mas_memory_page_create();
		return (page != 0);
	}

	void destroy() { mas_memory_page_free(page); }
};