#include "masStructDB.h"

#include <stdint.h>

// an idea that every struct has this above it consider it for exporting
//   its data and treat aliases as different struct having the same data layout
#define MAS_COMPONENT()

MAS_COMPONENT()
typedef union masVec2
{
	float xy[2];
	struct
	{
		float x, y;
	};
} masPoint;


MAS_COMPONENT()
typedef union masVec3
{
	float xyz[3];
	struct
	{
		float x, y, z;
	};
} masPosition, masScale, masRotation, masVelociy;


MAS_COMPONENT()
typedef union masVec4
{
	float xyzw[4];
	struct
	{
		float x, y, z, w;
	};
} masQuaternion;


void register_structs()
{
	MAS_REGISTER_STRUCT(masVec4,
		MAS_STRUCT_FIELD(float, x),
		MAS_STRUCT_FIELD(float, y),
		MAS_STRUCT_FIELD(float, z),
		MAS_STRUCT_FIELD(float, w));

	MAS_REGISTER_STRUCT(masVec3,
		MAS_STRUCT_FIELD(float, x),
		MAS_STRUCT_FIELD(float, y),
		MAS_STRUCT_FIELD(float, z));

	MAS_REGISTER_STRUCT(masVec2,
		MAS_STRUCT_FIELD(float, x),
		MAS_STRUCT_FIELD(float, y), );
}

void register_structs_aliases()
{
	MAS_REGISTER_STRUCT_ALIAS(masVec4,
		MAS_STRUCT_ALIAS(masQuaternion));

	MAS_REGISTER_STRUCT_ALIAS(masVec3,
		MAS_STRUCT_ALIAS(masPosition),
		MAS_STRUCT_ALIAS(masScale),
		MAS_STRUCT_ALIAS(masRotation),
		MAS_STRUCT_ALIAS(masVelociy));

	MAS_REGISTER_STRUCT_ALIAS(masVec2,
		MAS_STRUCT_ALIAS(masPoint));
}

int main(int argc, const char** argv)
{
	if (!masStructDB_Init())
		return -1;

	register_structs();
	register_structs_aliases();


	/*
	* query struct and inspect their data
	*/

	// struct to look for
	// their ids
	// their combination ids hash

	masStructDB_DeInit();
	return 0;
}


typedef struct masList
{
	uint32_t Idx;
	uint32_t Count;
};

///////////////////////////////////////////////////////////////////////////////////////////
// A ground for the above db to be used and tested
//     - All allocated memory for blocktype and block are ownd by memory system and
//       only having an indices to them
///////////////////////////////////////////////////////////////////////////////////////////
typedef union masObject
{
	uint64_t Handle;
	struct
	{
		uint32_t MapperIdx;
		uint32_t Generation;
	};
};

typedef struct masObjectMapper
{
	uint32_t Idx;
	uint32_t Generation;
	uint32_t BlockIdx;
	uint32_t BlockTypeIdx;
};

typedef struct masBlock
{
	uint8_t* Data;
	uint32_t ObjectCount;
};

typedef struct masBlockType
{
	masList  BlockList;
	masList  StructTypeList;
	uint32_t MaxObjectCount; // offsets are calculated based on structypelist element size * maxobjectcount to get the entire element array size
};

typedef struct masBlockTypeEntry
{
	uint64_t Hash; // calculated based on an object's combination of structs or data types
	uint32_t BlockTypeIdx;
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>

uint64_t mas_object()
{
	static uint64_t UniqueID = 1;
	return UniqueID++;
}

#define MAS_STRUCT(S)                   #S
#define MAS_DECL_STRUCT_LIST(Name, ...) const char* Name[] = { __VA_ARGS__ };
#define MAS_STRUCT_LIST(List)           List, sizeof(List)/sizeof(List[0])

void* mas_frame_malloc(size_t size)
{
	void* ptr = malloc(size);
	if (ptr)
		return false;
	memset(ptr, 0, size);
	return ptr;
}
#define MAS_FRAME_MALLOC(type, count) (type*)mas_frame_malloc(sizeof(type) * count)


bool mas_object_add_structs(uint64_t obj, const char** Structs, uint32_t Count)
{
	masStructQueryHeader* QueryHeader = masStructDB_Query(Structs, Count);
	if (!QueryHeader)
		return false;

	masBlockType* BlockType = mas_block_type_find(QueryHeader);
	if (!BlockType)
	{
		BlockType = mas_block_type_create(QueryHeader);
		if (!BlockType)
			return false;
	}

	mas_block_type_move_object(BlockType, obj);

#if 0
	for (int32_t StructIdx = 0; StructIdx < Count; ++StructIdx)
	{
		masStructQuery* Struct = masStructDB_QueryResult(QueryHeader, StructIdx);
		if (!Struct)
			return false;

		Struct->Name;
		Struct->UniqueID;
		Struct->Size;
		Struct->Alignment;

		for (int32_t FieldIdx = 0; FieldIdx < Struct->FieldCount; ++FieldIdx)
		{
			masFieldQuery* Field = &Struct->FieldList[FieldIdx];
			if (!Field)
				return false;

			Field->TypeID;
			Field->TypeName;
			Field->VarName;
			Field->Size;
			Field->Alignment;
			Field->Offset;
			Field->Flags;
		}
	}
#endif

	return true;
}

void test()
{
	uint64_t ent_node = mas_object();

	MAS_DECL_STRUCT_LIST(Transform,
		MAS_STRUCT(masPosition),
		MAS_STRUCT(masRotation),
		MAS_STRUCT(masScale));
	mas_object_add_structs(ent_node, MAS_STRUCT_LIST(Transform));
}