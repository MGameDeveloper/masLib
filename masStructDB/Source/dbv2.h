#pragma once

#include <stdint.h>
#include <stdlib.h>


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

typedef union masVec3
{
	float xyz[3];
	struct
	{
		float x, y, z;
	};
};

#include <string.h>
void w()
{
	const uint32_t buf_size = 1024;
	char buf[buf_size] = { };
	char* bufptr = buf;


	masRecord* rec = (masRecord*)bufptr;
	bufptr += sizeof(masRecord);

	rec->crc         = 0;
	rec->size        = 0;

	masStructRecord* srec = (masStructRecord*)rec->data;
	srec->unique_id    = 0;
	srec->alignment    = 0;
	srec->size         = 0;
	srec->name_len     = 0;
	srec->name_hash    = 0;
	srec->member_count = 0;
	memcpy(srec->name, 0, srec->name_len);

	bufptr += (sizeof(masStructRecord) + srec->name_len + 1);

	for (int32_t i = 0; i < srec->member_count; ++i)
	{
		masMemberRecord* mrec = (masMemberRecord*)bufptr;
		mrec->type     = 0;
		mrec->size     = 0;
		mrec->offset   = 0;
		mrec->name_len = 0;
		memcpy(mrec->name, 0, mrec->name_len);

		bufptr += (sizeof(masMemberRecord) + mrec->name_len + 1);
	}
}


typedef struct masMemberDesc
{
	const char *type;
	const char *name;
	uint32_t    size;
};
typedef struct masStructDesc
{
	const char    *name;
	masMemberDesc *members;
	uint32_t       alignment;
	uint32_t       size;
	uint32_t       member_count;
};

void mas_register_struct(masStructDesc* desc);

#define mas_array_size(a) (sizeof(a)/sizeof(a[0]))
#define mas_member(t, n)\
    {#t, #n, sizeof(t)}    
#define mas_struct(t, ...)\
    masMemberDesc t##members_desc[] = { __VA_ARGS__ };\
    masStructDesc t##struct_desc    = { #t, t##members_desc, sizeof(t), mas_array_size(t##members_desc)};\
    mas_register_struct(&t##struct_desc);


void l()
{
	mas_struct(masVec3,
		mas_member(float, x),
		mas_member(float, y),
		mas_member(float, z));
}


#define mas_ecs_frame_malloc(type, size) (type*)malloc(size)

void mas_register_struct(masStructDesc* desc)
{
	if (!desc)
		return;

	uint32_t record_size = sizeof(masRecord);	
	record_size += sizeof(masStructRecord) + strlen(desc->name) + 1;
	for (int32_t i = 0; i < desc->member_count; ++i)
		record_size += sizeof(masMemberRecord) + (strlen(desc->members[i].name) + 1);


	char* buf = mas_ecs_frame_malloc(char, record_size);
	if (!buf)
		return;
	char* bufptr = buf;


	masRecord* rec = (masRecord*)bufptr;
	rec->size = record_size;

	bufptr += sizeof(masRecord);
	masStructRecord* srec = (masStructRecord*)rec->data;
	srec->unique_id    = 0;
	srec->alignment    = desc->alignment;
	srec->size         = desc->size;
	srec->name_len     = strlen(desc->name);
	srec->name_hash    = 0;
	srec->member_count = desc->member_count;
	memcpy(srec->name, desc->name, srec->name_len);

	bufptr += (sizeof(masStructRecord) + srec->name_len + 1);
	for (int32_t i = 0; i < srec->member_count; ++i)
	{
		masMemberDesc   *mdesc = desc->members + i;
		masMemberRecord *mrec  = (masMemberRecord*)bufptr;

		mrec->type     = 0;
		mrec->size     = 0;
		mrec->offset   = 0;
		mrec->name_len = strlen(mdesc->name);
		memcpy(mrec->name, mdesc->name, mrec->name_len);

		bufptr += (sizeof(masMemberRecord) + mrec->name_len + 1);
	}

	rec->crc = 0;
	// mas_structdb_add_record(rec);
}