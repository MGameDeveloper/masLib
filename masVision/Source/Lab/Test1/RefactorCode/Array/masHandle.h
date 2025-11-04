#pragma once

#include <stdint.h>

union masHandle
{
	uint64_t Signiture;
	struct
	{
		uint64_t MapperIdx   : 24;
		uint64_t Version     : 12;
		uint64_t PoolID      : 12;
	};
};

