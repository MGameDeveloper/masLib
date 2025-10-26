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
	masHandle& operator=(const masHandle& Other) = delete; // Copy Semantic

public:
	masHandle();
	~masHandle()                            = default;
	masHandle(masHandle&& Other)            = default;
	masHandle& operator=(masHandle&& Other) = default;
	masHandle(const masHandle& Other)       = default; 

	bool operator==(const masHandle& Rhs);
};

#define MAS_DECLARE_HANDLE(Type)\
    class Type : masHandle      \
    {                           \
    }
