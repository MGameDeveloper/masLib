#pragma once

#include <stdint.h>

union masHandle
{
	uint64_t Signiture;
	struct
	{
		uint64_t Idx     : 24;
		uint64_t Version : 12;
		uint64_t Source  : 12;
	};
};