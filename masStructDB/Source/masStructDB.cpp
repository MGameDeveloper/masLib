#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FILE CONTENT HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct masFileContent
{
	void* Data;
	uint32_t Size;
};

bool masFileContent_Load(masFileContent* FileContent, const char* Path)
{
	FILE* File = fopen(Path, "rb");
	if (!File)
		return false;

	fseek(File, 0, SEEK_END);
	uint32_t FileSize = (uint32_t)_ftelli64(File);
	fseek(File, 0, SEEK_SET);
	if (FileSize <= 0)
	{
		fclose(File);
		return false;
	}

	void* Data = malloc((size_t)FileSize);
	if (!Data)
	{
		fclose(File);
		return false;
	}

	size_t ReadCount = fread(Data, 1, FileSize, File);
	if (ReadCount != FileSize)
	{
		fclose(File);
		free(Data);
		return false;
	}

	fclose(File);

	FileContent->Data = Data;
	FileContent->Size = FileSize;
	return true;
}

void masFileContent_UnLoad(masFileContent* FileContent)
{
	if (!FileContent)
		return;

	if (FileContent->Data)
		free(FileContent->Data);
	FileContent->Data = NULL;
	FileContent->Size = 0;
}

bool masFileContent_IsValid(masFileContent* FileContent)
{
	if (!FileContent || !FileContent->Data || FileContent->Size == 0)
		return false;
	return true;
}

bool masFileContent_WriteToDisk(masFileContent* FileContent, const char* Path)
{
	if (!masFileContent_IsValid(FileContent))
		return false;

	FILE* File = fopen(Path, "wb");
	if (!File)
		return false;

	size_t WriteByte = fwrite(FileContent->Data, 1, FileContent->Size, File);
	if (WriteByte != FileContent->Size)
	{
		// raise error
		fclose(File);
		return false;
	}

	if (fflush(File) == EOF)
	{
		fclose(File);
		return false;
	}

	fclose(File);
	return true;
}

void* masFileContent_GetDataPtr(masFileContent* FileContent, size_t Offset)
{
	if (!masFileContent_IsValid(FileContent) || (Offset >= FileContent->Size))
		return NULL;
	void* DataPointer = (((uint8_t*)FileContent->Data) + Offset);
	return DataPointer;
}

#define MAS_FILE_CONTENT_DATA_PTR(Type, FileContent, Offset) (Type*)masFileContent_GetDataPtr(FileContent, Offset)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FILE HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masFile_Exists(const char* Path)
{
	FILE* File = fopen(Path, "rb");
	if (!File)
		return false;
	fclose(File);
	return true;
}

bool masFile_Remove(const char* Path)
{
	if (remove(Path) != 0)
		return false;
	return true;
}

bool masFile_Rename(const char* CurrentName, const char* NewName)
{
	if (rename(CurrentName, NewName) != 0)
		return false;

	return true;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_TAG(c0, c1, c2, c3)           ((c0 << 24) | (c1 << 16) | (c2 << 8) | (c3 << 0))
#define MAS_LONG_TAG(t0, t1)              ((t0 << 32) | (t1 << 0))
#define MAS_STRUCTDB_TAG                   MAS_LONG_TAG(MAS_TAG('S', 'T', 'R', 'U'), MAS_TAG('C', 'T', 'D', 'B'))
#define MAS_STRUCTDB_FILENAME              "masStructDB.masDB"
#define MAS_STRUCTDB_BACKUP_FILENAME       "masStructDB.masDB_Backup"
#define MAS_STRUCTDB_TEMP_FILENAME	       "masStructDB.masDB_Temporary"
#define MAS_STRUCTDB_CORRUPTED_FILENAME    "masStructDB.masDB_Corrupted"
#define MAS_FIELD_INIT_COUNT                500000
#define MAS_STRUCT_INIT_COUNT               20000
#define MAS_NAME_BUFFER_INIT_COUNT          20000
#define MAS_HASH_ENTRY_INIT_COUNT           60000
#define MAS_AVARAGE_NAME_LEN                32


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STRUCT RELFECTION DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum masFieldType
{
	masFieldType_None,
	masFieldType_Int8,
	masFieldType_Int16,
	masFieldType_Int32,
	masFieldType_Int64,
	masFieldType_UInt8,
	masFieldType_UInt16,
	masFieldType_UInt32,
	masFieldType_UInt64,
	masFieldType_Char,
	masFieldType_String,
	masFieldType_Bool,

	masFieldType_Count
};

typedef enum masFieldFlag
{
	masFieldFlag_None   = 0,
	masFieldFlag_Const  = ( 1 << 0 ),
	masFieldFlag_Ptr    = ( 1 << 1 ),
};
#define MAS_FIELD_FLAGS(...) (masFieldFlag)(__VA_ARGS__)

typedef struct masDataRange
{
	uint32_t Index;
	uint32_t Count;
};

typedef struct masField
{
	masFieldType Type;
	masFieldFlag Flags;
	uint32_t     Size;
	masDataRange Name;
};

typedef struct masStruct
{
	uint32_t     UniqueID;
	uint32_t     Size;
	uint32_t     Alignment;
	masDataRange Name;
	masDataRange Fields;
};

typedef struct masHashEntry
{
	uint64_t Hash;
	uint64_t StructIdx;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STRUCT DATA BASE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum masChunkType
{
	masChunkType_StructList,
	masChunkType_FieldList,
	masChunkType_NameBuffer,
	masChunkType_HashTable,

	masChunkType_Count
};

typedef struct masChunkEntry
{
	uint32_t Offset;
	uint32_t Size;
};

typedef struct masStructDBHeader
{
	uint32_t Tag;
	uint32_t NextUniqueID;
};

typedef struct masStructDB
{
	masStructDBHeader *Header;
	masChunkEntry     *ChunkEntryList;
	masStruct      *StructList;
	masField       *FieldList;
	char              *NameBuffer;
	masHashEntry      *HashTable;
	masFileContent     File;
	bool               bUpdateDiskContent;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
static masStructDB GStructDB = { };


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERNAL HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool masStructDBInternal_Load()
{
	masFileContent* DBFile = &GStructDB.File;

	masStructDBHeader* Header = MAS_FILE_CONTENT_DATA_PTR(masStructDBHeader, DBFile, 0);
	if (!Header || Header->Tag != MAS_STRUCTDB_TAG)
		return false;

	masChunkEntry *ChunkEntryList = MAS_FILE_CONTENT_DATA_PTR(masChunkEntry, DBFile, sizeof(masStructDBHeader));
	masStruct  *StructList     = MAS_FILE_CONTENT_DATA_PTR(masStruct,  DBFile, ChunkEntryList[masChunkType_StructList].Offset);
	masField   *FieldList      = MAS_FILE_CONTENT_DATA_PTR(masField,   DBFile, ChunkEntryList[masChunkType_FieldList].Offset);
	char          *NameBuffer     = MAS_FILE_CONTENT_DATA_PTR(char,          DBFile, ChunkEntryList[masChunkType_NameBuffer].Offset);
	masHashEntry  *HashTable      = MAS_FILE_CONTENT_DATA_PTR(masHashEntry,  DBFile, ChunkEntryList[masChunkType_HashTable].Offset);

	if (!ChunkEntryList || !StructList || !FieldList || !NameBuffer || !HashTable)
		return false;

	GStructDB.Header         = Header;
	GStructDB.ChunkEntryList = ChunkEntryList;
	GStructDB.StructList     = StructList;
	GStructDB.FieldList      = FieldList;
	GStructDB.NameBuffer     = NameBuffer;
	GStructDB.HashTable      = HashTable;
	GStructDB.bUpdateDiskContent = false;

	return true;
}

static bool masStructDBInternal_Create()
{
	size_t ChunkEntrySizeList[masChunkType_Count] = { };
	ChunkEntrySizeList[masChunkType_StructList]   = MAS_STRUCT_INIT_COUNT      * sizeof(masStruct);
	ChunkEntrySizeList[masChunkType_FieldList]    = MAS_FIELD_INIT_COUNT       * sizeof(masField);
	ChunkEntrySizeList[masChunkType_NameBuffer]   = MAS_NAME_BUFFER_INIT_COUNT * MAS_AVARAGE_NAME_LEN;
	ChunkEntrySizeList[masChunkType_HashTable]    = MAS_HASH_ENTRY_INIT_COUNT  * sizeof(masHashEntry);

	size_t FileSize = sizeof(masStructDBHeader) + (masChunkType_Count * sizeof(masChunkEntry));
	for (int32_t i = 0; i < masChunkType_Count; ++i)
		FileSize += ChunkEntrySizeList[i];

	void* FileData = malloc(FileSize);
	if (!FileData)
		return false;
	memset(FileData, 0, FileSize);

	masStructDBHeader *Header = MAS_PTR_OFFSET(masStructDBHeader, FileData, 0);
	Header->Tag          = MAS_STRUCTDB_TAG;
	Header->NextUniqueID = 1;

	uint32_t       ChunkDataOffset = sizeof(masStructDBHeader) + (masChunkType_Count * sizeof(masChunkEntry));
	masChunkEntry *ChunkEntryList  = MAS_PTR_OFFSET(masChunkEntry, FileData, sizeof(masStructDBHeader));
	for (int32_t i = 0; i < masChunkType_Count; ++i)
	{
		ChunkEntryList[i].Offset = ChunkDataOffset;
		ChunkEntryList[i].Size   = ChunkEntrySizeList[i];

		ChunkDataOffset += ChunkEntrySizeList[i];
	}

	masStruct *StructList = MAS_PTR_OFFSET(masStruct, FileData, ChunkEntryList[masChunkType_StructList].Offset);
	masField  *FieldList  = MAS_PTR_OFFSET(masField,  FileData, ChunkEntryList[masChunkType_FieldList].Offset);
	char         *NameBuffer = MAS_PTR_OFFSET(char,         FileData, ChunkEntryList[masChunkType_NameBuffer].Offset);
	masHashEntry *HashTable  = MAS_PTR_OFFSET(masHashEntry, FileData, ChunkEntryList[masChunkType_HashTable].Offset);

	GStructDB.File.Data      = FileData;
	GStructDB.File.Size      = FileSize;
	GStructDB.Header         = Header;
	GStructDB.ChunkEntryList = ChunkEntryList;
	GStructDB.StructList     = StructList;
	GStructDB.FieldList      = FieldList;
	GStructDB.NameBuffer     = NameBuffer;
	GStructDB.HashTable      = HashTable;
	GStructDB.bUpdateDiskContent = false;

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATA BASE API
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masStructDB_Init()
{
	if (masFileContent_IsValid(&GStructDB.File))
		return true;

	if (!masFileContent_Load(&GStructDB.File, MAS_STRUCTDB_FILENAME))
		return false;

	if (!masStructDBInternal_Load())
	{
		if (!masStructDBInternal_Create())
		{
			masFileContent_UnLoad(&GStructDB.File);
			memset(&GStructDB, 0, sizeof(masStructDB));
			return false;
		}
	}

	return true;
}

void masStructDB_DeInit()
{
	masStructDB_Save();
	masFileContent_UnLoad(&GStructDB.File);
	memset(&GStructDB, 0, sizeof(masStructDB));
}

bool masStructDB_Save()
{
	if (!masFileContent_IsValid(&GStructDB.File))
		return false;

	if (masFileContent_WriteToDisk(&GStructDB.File, MAS_STRUCTDB_TEMP_FILENAME))
	{
		if (masFile_Exists(MAS_STRUCTDB_BACKUP_FILENAME))
			masFile_Remove(MAS_STRUCTDB_BACKUP_FILENAME);

		if (masFile_Rename(MAS_STRUCTDB_FILENAME, MAS_STRUCTDB_BACKUP_FILENAME))
			masFile_Rename(MAS_STRUCTDB_TEMP_FILENAME, MAS_STRUCTDB_FILENAME);
	}
	else
	{
		if (masFile_Exists(MAS_STRUCTDB_TEMP_FILENAME))
			masFile_Rename(MAS_STRUCTDB_TEMP_FILENAME, MAS_STRUCTDB_CORRUPTED_FILENAME);
		else
		{
			// raise error coulnt create temp file
		}
		return false;
	}

	return true;
}