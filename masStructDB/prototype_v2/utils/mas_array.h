#pragma once

#include "../mas_memory.h"

template<typename T>
class mas_array
{
private:
	mas_memory_array_id array;

public:
	mas_array(const mas_array&)            = delete;
	mas_array(mas_array&&)                 = delete;
	mas_array& operator=(const mas_array&) = delete;
	mas_array& operator=(mas_array&)       = delete;

public:
	mas_array() : array(0)
	{
	}

	~mas_array()
	{
		destroy();
	}

	bool create()
	{
		if (array.id == 0)
			array.id = mas_memory_array_create(sizeof(T));
		return (array.id != 0);
	}

	void add(const T* element)
	{
		void* arr_elem = mas_memory_array_new_element(array.id);
		if (arr_elem && element)
			mas_memory_copy(arr_elem, element, sizeof(T));
	}

	void   destroy()               { mas_memory_array_free(array.id);                        }
	T*     get(int32_t idx)        { return (T*)mas_memory_array_get_element(array.id, idx); }
	size_t count()                 { return mas_memory_array_element_count(array.id);        }
	size_t capacity()              { return mas_memory_array_capacity(array.id);             }
	bool   is_empty()              { return (count() == 0);                                  }
	void   clear()                 { mas_memory_array_clear(array.id);                       }
	T*     operator[](int32_t idx) { return get(idx);                                        }
};
