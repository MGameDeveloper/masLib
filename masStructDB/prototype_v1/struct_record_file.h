#pragma once

#include <stdint.h>

typedef struct masRecord
{
	uint32_t size;
	uint32_t crc;
	char     data[];
};


bool mas_record_file_init();
void mas_record_file_deinit();
bool mas_record_file_add(masRecord* rec);