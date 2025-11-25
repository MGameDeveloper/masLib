#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_COMPONENT_DB_TAG              (('C' << 24) | ('M' << 16) | ('P' << 8) | ('S' << 0))
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)

#define MAS_COMPONENT_DB_NAME           "masComponent.masDB"
#define MAS_COMPONENT_DB_BACKUP_NAME    "masComponent.masDB_Backup"
#define MAS_COMPONENT_DB_TEMP_NAME      "masComponent.masDB_Temp"
#define MAS_COMPONENT_DB_CORRUPTED_NAME "masComponent.masDB_Corrupted"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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
/*
* masComponent: Describe a component
*
* ID:   Unique ID of this component
* Size: Memory required size for this component
*
* Name: Since names' length vary doing fixed size name would cause waste of memory OR limit some name's if fixed name size is smaller
*       so all names live in a seperate block of memory where we only have:
*       - Offset: Offset from the start of name's block memory
*       - Length: Name's Length including NULL terminator
*/
typedef struct masComponent
{
	uint32_t ID;
	uint32_t Size;

	struct
	{
		uint32_t Offset;
		uint32_t Length;
	} Name;

	struct
	{
		uint32_t Index;
		uint32_t Count;
	}Fields;
};


/*
* masComponentEntry: Component's Name is hashed and truned to index to one of these
*
* Hash:    Result of hashing the component name USED to detect hashing collision
* CompIdx: Index into masComponent array
* _Pad:    Padding the structure manually
*/
typedef struct masComponentEntry
{
	uint64_t Hash;
	uint32_t CompIdx;
	uint32_t _Pad;
};


/*
* masComponentDBHeader: Describe data base on disk and where the data can be found within the file
*
* Tag:                  Magic number to distinguish data base of component from other files
* Version:              Has many use cases like detecting different implementation of the db[ currently not important ]
* ComponentGUID:        Keeps increasing to give every add component a unique id so they can be the same across machines and on different runs
* ComponentBlock:       Tells us where to find components' list on disk relative to the file
* NameBlock:            Tells us where to find components' name buffer on disk relative to the file
* ComponentEntryBlockc: Tells us where to find hash table for components on disk relative to the file
*/
typedef struct masComponentDBHeader
{
	uint32_t Tag;
	uint32_t Version;
	uint32_t ComponentGUID;
	uint32_t FileSize;

	struct
	{
		uint32_t Capacity;
		uint32_t Count;
		uint32_t DBOffset;
		uint32_t MemSize;
	} ComponentList;

	struct
	{
		uint32_t Capacity;
		uint32_t Count;
		uint32_t DBOffset;
		uint32_t MemSize;
	} NameBuffer;

	struct
	{
		uint32_t Capacity;
		uint32_t Count;
		uint32_t ResizeCounter;
		uint32_t DBOffset;
		uint32_t MemSize;
		float    LoadRatio;
	} ComponentEntryTable;
};


/*
* masComponentDB: Loaded components' data base from disk
*
* Header:              Parsed header where it has all data required to create or find data in the db
* ComponentList:       Pointer acquired after Mapping/Loading the data base from disk
* ComponentEntryTable: Hash table to find components' by their names
*/
typedef struct masComponentDB
{
	masFileContent DBFile;

	masComponentDBHeader* Header;
	masComponent* ComponentList;
	masComponentEntry* ComponentEntryTable;
	char* NameBuffer;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
static masComponentDB GComponentDB = { };


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masComponentDBInternal_Create()
{
	uint32_t InitCompCapacity = 256;
	uint32_t InitNameBufferCapacity = 1024;
	uint32_t InitCompEntryCapacity = 1024;

	uint32_t DBFileSize =
		sizeof(masComponentDBHeader) +
		(sizeof(masComponent) * InitCompCapacity) +
		(sizeof(masComponentEntry) * InitCompEntryCapacity) +
		InitNameBufferCapacity;

	void* DBData = malloc(DBFileSize);
	if (!DBData)
		return false;
	memset(DBData, 0, DBFileSize);

	masComponentDBHeader* Header = MAS_PTR_OFFSET(masComponentDBHeader, DBData, 0);
	Header->Tag = MAS_COMPONENT_DB_TAG;
	Header->Version = 1;
	Header->ComponentGUID = 1;
	Header->FileSize = DBFileSize;

	Header->ComponentList.Capacity = InitCompCapacity;
	Header->ComponentList.Count = 0;
	Header->ComponentList.DBOffset = sizeof(masComponentDBHeader);
	Header->ComponentList.MemSize = InitCompCapacity * sizeof(masComponent);

	Header->NameBuffer.Capacity = InitNameBufferCapacity;
	Header->NameBuffer.Count = 0;
	Header->NameBuffer.DBOffset = Header->ComponentList.DBOffset + Header->ComponentList.MemSize;
	Header->NameBuffer.MemSize = sizeof(char) * InitNameBufferCapacity;

	Header->ComponentEntryTable.Capacity = InitCompEntryCapacity;
	Header->ComponentEntryTable.Count = 0;
	Header->ComponentEntryTable.DBOffset = Header->NameBuffer.DBOffset + Header->NameBuffer.MemSize;
	Header->ComponentEntryTable.MemSize = sizeof(masComponentEntry) * InitCompEntryCapacity;

	GComponentDB.DBFile.Data = DBData;
	GComponentDB.DBFile.Size = DBFileSize;
	GComponentDB.Header = MAS_PTR_OFFSET(masComponentDBHeader, GComponentDB.DBFile.Data, 0);
	GComponentDB.ComponentList = MAS_PTR_OFFSET(masComponent, GComponentDB.DBFile.Data, Header->ComponentList.DBOffset);
	GComponentDB.NameBuffer = MAS_PTR_OFFSET(char, GComponentDB.DBFile.Data, Header->NameBuffer.DBOffset);
	GComponentDB.ComponentEntryTable = MAS_PTR_OFFSET(masComponentEntry, GComponentDB.DBFile.Data, Header->ComponentEntryTable.DBOffset);

	return true;
}

bool masComponentDBInternal_Load()
{
	if (!masFileContent_IsValid(&GComponentDB.DBFile) || GComponentDB.DBFile.Size <= sizeof(masComponentDBHeader))
		return false;

	masComponentDBHeader* Header = MAS_PTR_OFFSET(masComponentDBHeader, GComponentDB.DBFile.Data, 0);
	if (Header->Tag != MAS_COMPONENT_DB_TAG)
	{
		// raise an error file is not component data base
		return false;
	}

	// Header.Version; // use version if needed

	GComponentDB.Header = MAS_PTR_OFFSET(masComponentDBHeader, GComponentDB.DBFile.Data, 0);
	GComponentDB.ComponentList = MAS_PTR_OFFSET(masComponent, GComponentDB.DBFile.Data, Header->ComponentList.DBOffset);
	GComponentDB.NameBuffer = MAS_PTR_OFFSET(char, GComponentDB.DBFile.Data, Header->NameBuffer.DBOffset);
	GComponentDB.ComponentEntryTable = MAS_PTR_OFFSET(masComponentEntry, GComponentDB.DBFile.Data, Header->ComponentEntryTable.DBOffset);

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ComponentDB Api
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masComponentDB_Create()
{
	return masComponentDBInternal_Create();
}

bool masComponentDB_Load()
{
	if (!masFileContent_Load(&GComponentDB.DBFile, MAS_COMPONENT_DB_NAME))
		return false;

	if (!masComponentDBInternal_Load())
	{
		masFileContent_UnLoad(&GComponentDB.DBFile);
		return false;
	}

	return true;
}

bool masComponentDB_Save()
{
	if (!masFileContent_IsValid(&GComponentDB.DBFile))
		return false;

	if (masFileContent_WriteToDisk(&GComponentDB.DBFile, MAS_COMPONENT_DB_TEMP_NAME))
	{
		if (masFile_Exists(MAS_COMPONENT_DB_BACKUP_NAME))
			masFile_Remove(MAS_COMPONENT_DB_BACKUP_NAME);

		if (masFile_Rename(MAS_COMPONENT_DB_NAME, MAS_COMPONENT_DB_BACKUP_NAME))
			masFile_Rename(MAS_COMPONENT_DB_TEMP_NAME, MAS_COMPONENT_DB_NAME);
	}
	else
	{
		if (masFile_Exists(MAS_COMPONENT_DB_TEMP_NAME))
			masFile_Rename(MAS_COMPONENT_DB_TEMP_NAME, MAS_COMPONENT_DB_CORRUPTED_NAME);
		else
		{
			// raise error coulnt create temp file
		}
		return false;
	}

	return true;
}

void masComponentDB_Destroy()
{
	masFileContent_UnLoad(&GComponentDB.DBFile);
	memset(&GComponentDB, 0, sizeof(masComponentDB));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Structs Reflection Data: NEW DESIGN OF THE ABOVE DB
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
	uint32_t     UniqueID;
	masFieldType Type;
	masFieldFlag Flags;
	uint32_t     Size;
	masDataRange Name;
};

typedef struct masStruct
{
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

#define MAS_FIELD_INIT_COUNT       500000
#define MAS_STRUCT_INIT_COUNT      20000
#define MAS_HASH_ENTRY_INIT_COUNT  60000
#define MAS_NAME_BUFFER_INIT_COUNT 20000


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Structs Reflection DB
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum masChunkType
{
	masChunkType_StructList,
	masChunkType_FieldList,
	masChunkType_NameBuffer,
	masChunkType_HashTable,
};

typedef struct masContentTableEntry
{
	uint64_t Offset;
	uint32_t Type;
	uint32_t ElementSize;
	uint32_t ElementCount;
	uint32_t _Pad;
};

typedef struct masStructDB
{
	uint64_t Magic;
	uint64_t FileSize;
	uint64_t ContentTableOffset;
	uint32_t ContentTableSize;
	uint32_t Version;
	uint32_t _Pad[2];
};