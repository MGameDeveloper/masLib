#include "struct_registery.h"
#include "../Source/masECSMemory.h"
#include <stdio.h>
#include <string.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum masDataType
{
	masData_Unknown,

	masData_Int8,
	masData_Int16,
	masData_Int32,
	masData_Int64,
	masData_UInt8,
	masData_UInt16,
	masData_UInt32,
	masData_UInt64,
	masData_Bool,
	masData_Char,
	masData_Void,
	masData_Float,
	masData_Double,
};

typedef struct masMemberRecord
{
	uint32_t type;
	uint32_t size;
	uint32_t offset;
	uint32_t name_len;
	char     name[];
};

typedef struct masStructRecord
{
	uint64_t name_hash;
	uint32_t unique_id;
	uint32_t size;
	uint32_t alignment;
	uint32_t member_count;
	uint32_t name_len;
	char     name[];
};

typedef struct masRecord
{
	uint32_t size;
	uint32_t crc;
	char     data[];
};

typedef struct masRecordHeader
{
	uint32_t tag;
	uint32_t unique_id_gen;
};

typedef struct masRecordFile
{
	
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
uint64_t mas_internal_hash(const void* data, size_t size)
{
	const uint8_t* bytes = (const uint8_t*)data;
	uint64_t h = 0xcbf29ce484222325ULL; // offset basis
	const uint64_t prime = 0x100000001b3ULL;
	for (size_t i = 0; i < size; ++i) {
		h ^= bytes[i];
		h *= prime;
	}
	return h;
}

uint32_t mas_internal_calc_crc32(const void* data, size_t size)
{
	const uint8_t* buf = (const uint8_t*)data;
	const uint32_t poly = 0xEDB88320u;      // reflected polynomial
	uint32_t crc = 0xFFFFFFFFu;             // initial value

	while (size--) {
		crc ^= *buf++;
		for (int k = 0; k < 8; ++k) {
			if (crc & 1u) crc = (crc >> 1) ^ poly;
			else crc >>= 1;
		}
	}
	return crc ^ 0xFFFFFFFFu;               // final XOR
}

masDataType mas_internal_convert_typename(const char* type)
{
#define eq(t, n) (strcmp(t, #n) == 0)

	if (eq(type, float))
		return masData_Float;
	else if (eq(type, double))
		return masData_Double;
	else if (eq(type, bool))
		return masData_Bool;
	else if (eq(type, char))
		return masData_Char;

	// signed
	else if (eq(type, int8_t))
		return masData_Int8;
	else if (eq(type, int16_t) || eq(type, short))
		return masData_Int16;
	else if (eq(type, int32_t) || eq(type, int) || eq(type, long))
		return masData_Int32;
	else if (eq(type, int64_t) || eq(type, long long))
		return masData_Int64;

	// unsigned 
	else if (eq(type, uint8_t))
		return masData_UInt8;
	else if (eq(type, uint16_t) || eq(type, unsigned short))
		return masData_UInt16;
	else if (eq(type, uint32_t) || eq(type, unsigned int) || eq(type, unsigned long))
		return masData_UInt32;
	else if (eq(type, uint64_t) || eq(type, unsigned long long))
		return masData_UInt64;

#undef eq
	return masData_Unknown;
}

bool mas_record_file_add(masRecord* rec)
{
	const char* file_path = "";

	FILE* file = fopen(file_path, "wb");
	if (!file)
		return false;

	size_t byte = fwrite(rec, 1, rec->size, file);
	if (byte != rec->size)
		return false;

	// mas_commit_record(rec);

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool mas_struct_registery_init()
{

}

void mas_struct_registery_deinit()
{

}

void mas_sttrct_registery_add(masStructDesc* desc)
{
	if (!desc)
		return;

	// calculate entire record size including header + struct + members
	uint32_t record_size = sizeof(masRecord);
	record_size += sizeof(masStructRecord) + strlen(desc->name) + 1;
	for (int32_t i = 0; i < desc->member_count; ++i)
		record_size += sizeof(masMemberRecord) + (strlen(desc->members[i].name) + 1);

	// alloc using frame allocator 
	char* buf = MAS_ECS_FRAME_MALLOC(char, record_size);
	if (!buf)
		return;
	char* bufptr = buf;


	masRecord* rec = (masRecord*)bufptr;
	rec->size = record_size;

	bufptr += sizeof(masRecord);
	masStructRecord* srec = (masStructRecord*)rec->data;
	srec->unique_id = 0;
	srec->alignment = desc->alignment;
	srec->size = desc->size;
	srec->name_len = strlen(desc->name);
	srec->name_hash = mas_internal_hash(desc->name, srec->name_len);
	srec->member_count = desc->member_count;
	memcpy(srec->name, desc->name, srec->name_len);

	bufptr += (sizeof(masStructRecord) + srec->name_len + 1);
	uint32_t member_offset = 0;
	for (int32_t i = 0; i < srec->member_count; ++i)
	{
		masMemberDesc* mdesc = desc->members + i;
		masMemberRecord* mrec = (masMemberRecord*)bufptr;

		mrec->type = mas_internal_convert_typename(mdesc->name);
		mrec->size = mdesc->size;
		mrec->offset = member_offset;
		mrec->name_len = strlen(mdesc->name);
		memcpy(mrec->name, mdesc->name, mrec->name_len);

		bufptr += (sizeof(masMemberRecord) + mrec->name_len + 1);
		member_offset += mrec->size;
	}

	rec->crc = mas_internal_calc_crc32(rec, rec->size);
	
	if (!mas_record_file_add(rec))
	{
		// log error
	}
}