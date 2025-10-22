#pragma once

#include <stdint.h>

struct masResourceDef
{
	int32_t  RefCount;
	uint32_t Type;
	char     Name[256];
};

