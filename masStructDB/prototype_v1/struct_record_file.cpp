#include "struct_record_file.h"
#include <stdio.h>
#include <string.h>

typedef union mas4CCTag
{
	char c4[4];
	struct
	{
		uint32_t Tag;
	};
};

typedef struct masRecordHeader
{
	uint32_t tag;
	uint32_t unique_id_gen;
	uint32_t write_offset;
	uint32_t read_offset;
};

static FILE* grecfile = NULL;

bool mas_record_file_init()
{
	if (grecfile)
		return true;

	const char* path = "struct_record.masDBRec";
	FILE *file = fopen(path, "wb");
	if (!file)
		return false;

	mas4CCTag Tag = {};
	Tag.c4[0] = 'r';
	Tag.c4[1] = 'e';
	Tag.c4[2] = 'c';
	Tag.c4[3] = 'd';

	masRecordHeader Header = { };
	Header.tag             = Tag.Tag;
	Header.unique_id_gen   = 1;
	Header.read_offset     = 0;
	Header.write_offset    = 0;

	size_t byte = fwrite(&Header, 1, sizeof(Header), grecfile);
	if (byte != sizeof(Header))
	{
		mas_record_file_deinit();
		return false;
	}

	return true;
}

void mas_record_file_deinit()
{
	if (GRecord.file)
	{
		fclose(GRecord.file);
		memset(&GRecord, 0, sizeof(masRecordFile));
	}
}

bool mas_record_file_add(masRecord* rec)
{
	if (!GRecord.file)
		return false;
}