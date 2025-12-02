#include <stdio.h>
#include <string.h>

#include "mas_ecs_struct_registery.h"
#include "mas_ecs_memory.h"
#include "mas_mmap.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_RECORD_TAG (( 'R' << 24) | ( 'E' << 16) | ( 'C' << 8) | ( 'D' << 0))
#define MAS_HASH_TABLE_TAG (( 'H' << 24) | ( 'S' << 16) | ( 'H' << 8) | ( 'T' << 0))
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))


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
	uint32_t crc;
	uint32_t size;
	char     data[];
};

typedef struct masRecordHeader
{
	uint32_t tag;
	uint32_t unique_id_gen;
	uint32_t record_count;
	uint32_t write_offset;
};

typedef struct masHashTableHeader
{
	uint32_t tag;
	uint32_t capacity;
	uint32_t count;
	float    load_factor;
};



///////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
static mas_mmap_t       g_rec_file   = { };
static mas_mmap_t       g_hash_table = { };
static masRecordHeader *g_rec_hdr    = NULL;
static masHashTableHeader* g_hash_table_hdr = NULL;


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

const char* mas_internal_convert_type_to_name(masDataType type)
{
	switch (type)
	{
	case masData_Int8:   return "int8_t";
	case masData_Int16:  return "int16_t";
	case masData_Int32:  return "int32_t";
	case masData_Int64:  return "int64_t";
	case masData_UInt8:  return "uint8_t";
	case masData_UInt16: return "uint16_t";
	case masData_UInt32: return "uint32_t";
	case masData_UInt64: return "uint64_t";
	case masData_Bool:   return "bool";
	case masData_Char:   return "char";
	case masData_Void:   return "void";
	case masData_Float:  return "float";
	case masData_Double: return "double";
	}

	return "MAS_DATA_UNKNOWN_TYPE";
}

bool mas_internal_map_record_file()
{
	mas_mmap_ret_t mmap_ret = mas_mmap(&g_rec_file, "mas_struct_records.masDBRec");
	if (mmap_ret == mas_mmap_ret_error)
		return false;

	g_rec_hdr = (masRecordHeader*)mas_mmap_read(&g_rec_file, 0, sizeof(masRecordHeader));
	if (!g_rec_hdr)
	{
		mas_unmmap(&g_rec_file);
		return false;
	}

	if (mmap_ret == mas_mmap_ret_created)
	{
		g_rec_hdr->tag = MAS_RECORD_TAG;
		g_rec_hdr->unique_id_gen = 1;
		g_rec_hdr->write_offset += sizeof(masRecordHeader);
	}
	else if (mmap_ret == mas_mmap_ret_opened)
	{
		if (g_rec_hdr->tag != MAS_RECORD_TAG)
		{
			// log error not a record db
			mas_unmmap(&g_rec_file);
			return false;
		}
	}

	return true;
}

const masStructRecord* mas_internal_find_struct(uint64_t name_hash)
{
	if (!mas_mmap_is_valid(&g_rec_file))
		return NULL;

	uint32_t read_offset = sizeof(masRecordHeader);
	for (int32_t i = 0; i < g_rec_hdr->record_count; ++i)
	{
		masRecord       *rec  = (masRecord*)mas_mmap_read(&g_rec_file, read_offset, sizeof(masRecord));
		masStructRecord *srec = (masStructRecord*)rec->data;
		if (srec->name_hash == name_hash)
			return srec;

		read_offset += rec->size;
	}

	return NULL;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool mas_struct_registery_init()
{
	if (!mas_internal_map_record_file())
		return false;

	return true;
}

void mas_struct_registery_deinit()
{
	mas_unmmap(&g_rec_file);
}

bool mas_struct_registery_add(mas_struct_desc* desc)
{
	if (!desc)
		return false;

	size_t   name_len  = strlen(desc->name);
	uint64_t name_hash = mas_internal_hash(desc->name, name_len);
	if (mas_internal_find_struct(name_hash))
		return true;

	// calculate entire record size including header + struct + members
	uint32_t record_size = sizeof(masRecord);
	record_size += sizeof(masStructRecord) + strlen(desc->name) + 1;
	for (int32_t i = 0; i < desc->member_count; ++i)
		record_size += sizeof(masMemberRecord) + (strlen(desc->members[i].name) + 1);

	// alloc using frame allocator 
	char* buf = MAS_ECS_MEMORY_FRAME_MALLOC(char, record_size);
	if (!buf)
		return false;
	char* bufptr = buf;


	masRecord* rec = (masRecord*)bufptr;
	rec->size = record_size;

	bufptr += sizeof(masRecord);
	masStructRecord* srec = (masStructRecord*)rec->data;
	srec->unique_id    = g_rec_hdr->unique_id_gen;
	srec->alignment    = desc->alignment;
	srec->size         = desc->size;
	srec->name_len     = name_len;
	srec->name_hash    = name_hash;
	srec->member_count = desc->member_count;
	memcpy(srec->name, desc->name, srec->name_len);

	bufptr += (sizeof(masStructRecord) + srec->name_len + 1);
	uint32_t member_offset = 0;
	for (int32_t i = 0; i < srec->member_count; ++i)
	{
		mas_struct_member_desc* mdesc = desc->members + i;
		masMemberRecord* mrec = (masMemberRecord*)bufptr;

		mrec->type        = mas_internal_convert_typename(mdesc->type);
		mrec->size        = mdesc->size;
		mrec->offset      = member_offset;
		mrec->name_len    = strlen(mdesc->name);
		memcpy(mrec->name, mdesc->name, mrec->name_len);

		bufptr += (sizeof(masMemberRecord) + mrec->name_len + 1);
		member_offset += mrec->size;
	}

	void* rec_block = MAS_PTR_OFFSET(void, rec, sizeof(uint32_t)); // we exclude the crc from contributing to the final value
	rec->crc = mas_internal_calc_crc32(rec_block, rec->size - sizeof(uint32_t));
	

	size_t bytes = mas_mmap_write(&g_rec_file, g_rec_hdr->write_offset, rec, rec->size);
	if (bytes != rec->size)
	{
		// log error
		return false;
	}
	g_rec_hdr->write_offset += bytes;
	g_rec_hdr->unique_id_gen++;
	g_rec_hdr->record_count++;

	return true;
}

void mas_struct_registery_print()
{
	masRecordHeader* hdr = (masRecordHeader*)mas_mmap_read(&g_rec_file, 0, sizeof(masRecordHeader));
	printf("RECORD_DB:\n");
	printf("    - TAG:       %u\n", hdr->tag);
	printf("    - GUID:      %u\n", hdr->unique_id_gen);
	printf("    - REC_COUNT: %u\n", hdr->record_count);
	printf("    - WritePtr:  %u\n", hdr->write_offset);
	printf("    - RECORDS[]:\n");

	uint32_t read_offset = sizeof(masRecordHeader);
	for (int32_t i = 0; i < hdr->record_count; ++i)
	{
		printf("\n        [ STRUCT_RECORD ]\n");

		masRecord* rec = (masRecord*)mas_mmap_read(&g_rec_file, read_offset, sizeof(masRecord));
		void* rec_block = MAS_PTR_OFFSET(void, rec, sizeof(uint32_t));
		printf("        - CRC_CHECK: %u\n", mas_internal_calc_crc32(rec_block, rec->size - sizeof(uint32_t)));
		printf("        - CRC:       %u\n", rec->crc);
		printf("        - SIZE:      %u\n", rec->size);
		
		masStructRecord* srec = (masStructRecord*)rec->data;
		printf("        - NAME:         %s\n", srec->name);
		printf("        - ID:           %u\n", srec->unique_id);
		printf("        - SIZE:         %u\n", srec->size);
		printf("        - ALIGNMENT:    %u\n", srec->alignment);
		printf("        - MEMBER_COUNT: %u\n", srec->member_count);
		printf("        - MEMBERS[]:\n");

		uint32_t member_read_offset = sizeof(masStructRecord) + (srec->name_len + 1);
		for (int32_t m = 0; m < srec->member_count; ++m)
		{
			masMemberRecord* mrec = MAS_PTR_OFFSET(masMemberRecord, srec, member_read_offset);
			printf("            - %s %s; [ %u | %u ]\n",
				mas_internal_convert_type_to_name((masDataType)mrec->type),
				mrec->name,
				mrec->size, 
				mrec->offset);

			member_read_offset += sizeof(masMemberRecord) + (mrec->name_len + 1);
		}

		read_offset += rec->size;
	}
}

bool mas_struct_find_structs(const char** names, uint32_t* out_ids, uint32_t count)
{
	if (!names || !(*names) || !out_ids || count == 0)
		return false;

	for (int32_t i = 0; i < count; ++i)
	{
		uint64_t struct_hash = mas_internal_hash(names[i], strlen(names[i]));
		const masStructRecord* srec = mas_internal_find_struct(struct_hash);
		if (!srec)
			return false;
		out_ids[i] = srec->unique_id;
	}

	return true;
}