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
	mas_array()
	{
		array.id = 0;
	}

	~mas_array()
	{

	}

	bool create(uint32_t capacity = 2)
	{
		if (array.id == 0)
		{
			array = mas_memory_array_create(sizeof(T));
			mas_memory_array_reserve(array, capacity);
		}
		return (array.id != 0);
	}

	void add(const T* element)
	{
		void* arr_elem = mas_memory_array_new_element(array);
		if (arr_elem && element)
			mas_memory_copy(arr_elem, element, sizeof(T));
	}

	T*     new_element()           { return (T*)mas_memory_array_new_element(array);      }
	void   destroy()               { mas_memory_array_free(array);                        }
	size_t count()                 { return mas_memory_array_element_count(array);        }
	size_t capacity()              { return mas_memory_array_capacity(array);             }
	bool   is_empty()              { return (count() == 0);                               }
	void   clear()                 { mas_memory_array_clear(array);                       }
	bool   is_valid()              { return mas_memory_array_is_valid(array);             }

	T* get_element(int32_t idx)
	{
		if (idx < 0)
			return NULL;
		return (T*)mas_memory_array_get_element(array, idx);
	}

	void set_element(const T* element, int32_t idx)
	{
		T* arr_elem = get_element(idx);
		if (!arr_elem)
			return;
		mas_memory_copy(arr_elem, element, sizeof(T));
	}
};
