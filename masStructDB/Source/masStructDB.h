#pragma once


bool masStructDB_Init();
void masStructDB_DeInit();
bool masStructDB_Save();


/*
* FOR TESTING WILL IMPLEMENT MANUAL REGISTERATION 
*     - LATER WRITE A SCANNER AS PRECOMPILE BUILD STEP THAT SCAN
*       SOURCES & HEADERS AND POPULATE THE DATA BASE
*/

struct masStructField
{
	const char *Type;
	const char *Name;
	size_t      Size;
	size_t      Offset;
};

void masStructDB_RegisterStruct(const char* Name, masStructField* Fields, size_t Count);
void masStructDB_RegisterStructAliases(const char* Name, const char** Aliases, size_t Count); // if name has not been registered ignore and return aliases


#define MAS_STRUCT_FIELD(type, name) {#type, #name, sizeof(type), 0}
#define MAS_ARRAY_SIZE(a)            (sizeof(a)/sizeof(a[0]))
#define MAS_STRUCT_ALIAS(type)       #type

#define MAS_REGISTER_STRUCT(Type, ...)                             \
    do                                                             \
	{                                                              \
        masStructField Fields[] = { __VA_ARGS__ };                 \
        size_t         Count    = sizeof(Fields)/sizeof(Fields[0]);\
        masStructDB_RegisterStruct(#Type, Fields, Count);          \
	}while(0)

#define MAS_REGISTER_STRUCT_ALIAS(Type, ...)                       \
    do                                                             \
    {                                                              \
        const char* Aliases[] = { __VA_ARGS__ };                   \
        size_t      Count     = sizeof(Aliases)/sizeof(Aliases[0]);\
        masStructDB_RegisterStructAliases(#Type, Aliases, Count);  \
    }while(0)

typedef union masVec3
{
	float xyz[3];
	struct
	{
		float x, y, z;
	};
} masPosition, masScale, masRotation, masVelociy;

void j()
{
	MAS_REGISTER_STRUCT(masVec3,
		MAS_STRUCT_FIELD(float, x),
		MAS_STRUCT_FIELD(float, y),
		MAS_STRUCT_FIELD(float, z));

	MAS_REGISTER_STRUCT_ALIAS(masVec3,
		MAS_STRUCT_ALIAS(masPosition),
		MAS_STRUCT_ALIAS(masScale),
		MAS_STRUCT_ALIAS(masRotation),
		MAS_STRUCT_ALIAS(masVelociy));
}


