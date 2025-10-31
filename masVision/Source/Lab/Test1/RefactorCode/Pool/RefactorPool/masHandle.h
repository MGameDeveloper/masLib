#pragma once

#include <stdint.h>

struct masHandle
{
	union
	{
		uint32_t Signiture;
		uint32_t Idx     : 17;
		uint32_t Version : 8;
		uint32_t Source  : 7;
	};

	masHandle() : Signiture(0) { }
};


#define MAS_DECLARE_HANDLE(Name)     \
struct Name                          \
{                                    \
    masHandle Handle;                \
    Name();                          \
    ~Name();                         \
    Name(const Name& Other);         \
    Name& operator=(const Name& Rhs);\
    Name(Name&& Other);              \
    Name& operator=(Name&& Rhs);     \
}

#define MAS_DECLARE_HANDLE_ARRAY(Name)\
    struct Name\
    {\
        uint32_t Idx;\
        uint32_t Count;\
    }
