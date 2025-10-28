#pragma once

#include <stdint.h>

class masHandle
{
private:
	friend class masRawPool;

	union
	{
		uint32_t Sig;
		uint32_t SlotIdx : 16;
		uint32_t GenID   : 8;
		uint32_t PoolID  : 7;
	};

public:
	masHandle();
	~masHandle()                               = default;
	masHandle(const masHandle& Other)          = default; 
	masHandle(masHandle&& Other)               = default;
	masHandle& operator=(const masHandle& Rhs) = default;
	masHandle& operator=(masHandle&& Rhs)      = default;

	bool operator==(const masHandle& Rhs);
};

#define MAS_DECLARE_HANDLE(Type)\
    class Type : masHandle      \
    {                           \
    }
