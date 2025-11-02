#pragma once

#include <stdint.h>

union masHandle
{
	uint64_t Signiture;
	struct
	{
		uint64_t SlotIdx     : 24;
		uint64_t Version     : 12;
		uint64_t PoolID      : 12;
		uint64_t HasChildren : 1;
		uint64_t HasParent   : 1;
	};
};

