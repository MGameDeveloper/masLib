#pragma once

#include "../mas_memory.h"

template<typename T>
class mas_stack
{
private:
	mas_memory_stack_id stack;

public:
	mas_stack(const mas_stack&)            = delete;
	mas_stack(mas_stack&&)                 = delete;
	mas_stack& operator=(const mas_stack&) = delete;
	mas_stack& operator=(mas_stack&)       = delete;

public:
	mas_stack()
	{ 
		stack.id = 0;
	}

	~mas_stack()
	{

	}

	bool create() 
	{ 
		if(stack.id == 0)
			stack = mas_memory_stack_create(sizeof(T));
		return (stack.id != 0);
	}

	void push(const T* item)
	{
		void* stack_elm = mas_memory_stack_push_element(stack);
		if (stack_elm && item)
			mas_memory_copy(stack_elm, item, sizeof(T));
	}

	void destroy()  { mas_memory_stack_free(stack);               }
	T*   top()      { return (T*)mas_memory_stack_top_element(stack); }
	bool is_empty() { return mas_memory_stack_is_empty(stack);    }
	void pop()      { mas_memory_stack_pop_element(stack);        }
	bool is_valid() { return mas_memory_stack_is_valid(stack);           }
};