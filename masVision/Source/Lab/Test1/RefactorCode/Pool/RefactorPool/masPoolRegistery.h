#pragma once

#include <stdint.h>

struct masPoolInfo
{
	struct masPool* Pool;
	int32_t *Capacity;
	int32_t *Size;
	int32_t  ElementSize;
	char     Name[128];

};

void masPoolRegistery_Add(masPoolInfo* PoolInfo);
