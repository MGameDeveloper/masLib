#pragma once

#include <stdint.h>


bool masStructDB_Init();
void masStructDB_DeInit();
bool masStructDB_Save();


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Register API
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct masStructField
{
	const char *TypeName;
	const char *Name;
	size_t      Size;
	size_t      Offset;
};

void masStructDB_RegisterStruct(const char* Name, size_t Size, masStructField* Fields, size_t Count);
void masStructDB_RegisterStructAliases(const char* Name, const char** Aliases, size_t Count);

#define MAS_STRUCT_FIELD(type, name) {#type, #name, sizeof(type), 0}
#define MAS_ARRAY_SIZE(a)            (sizeof(a)/sizeof(a[0]))
#define MAS_STRUCT_ALIAS(type)       #type

#define MAS_REGISTER_STRUCT(Type, ...)                                 \
    do                                                                 \
	{                                                                  \
        masStructField Fields[] = { __VA_ARGS__ };                     \
        size_t         Count    = sizeof(Fields)/sizeof(Fields[0]);    \
        masStructDB_RegisterStruct(#Type, sizeof(Type), Fields, Count);\
	}while(0)

#define MAS_REGISTER_STRUCT_ALIAS(Type, ...)                       \
    do                                                             \
    {                                                              \
        const char* Aliases[] = { __VA_ARGS__ };                   \
        size_t      Count     = sizeof(Aliases)/sizeof(Aliases[0]);\
        masStructDB_RegisterStructAliases(#Type, Aliases, Count);  \
    }while(0)

#define MAS_REGISTER_TAG(Tag)\
    masStructDB_RegisterStruct(#Tag, sizeof(Tag), NULL, 0)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Query API
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct masFieldQuery
{
    const char* VarName;
    const char* TypeName;
    uint32_t    TypeID;
    uint32_t    Size;
    uint32_t    Alignment;
    uint32_t    Offset;
    uint32_t    Flags;
};

typedef struct masStructQuery
{
    masFieldQuery *FieldList;
    const char    *Name;
    uint32_t       UniqueID;
    uint32_t       Size;
    uint32_t       Alignment;
    uint32_t       FieldCount;
};

typedef struct masStructQueryHeader
{
    uint32_t *OffsetList;
    uint32_t  Count;
};

// Query Result lives on temporary memory that got zeroed every frame when it's dirty
masStructQueryHeader* masStructDB_Query(const char** Structs, uint32_t Count);
masStructQuery*       masStructDB_QueryResult(masStructQueryHeader* QueryHeader, uint32_t Idx);