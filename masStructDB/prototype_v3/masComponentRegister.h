#pragma once


bool masComponentRegister_Init();
void masComponentRegister_DeInit();


/////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////
struct masComponent
{
	size_t Hash;
	size_t Size;
	size_t UniqueID;
	char   Name[32];
};

void masComponentRegister_Add(const char* Name, size_t Size);


/////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////
struct masComponentList
{
	const char** Names;
	size_t       Count;
};

struct masComponentQuery
{
	masComponent* Components;
	size_t Count;
	size_t Hash;
};

masComponentQuery* masComponentRegister_Query(masComponentList* CompList);


/////////////////////////////////////////////////////////////////////////////////////////////////
// HELPER MACROS
/////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_COMPONENT_ADD(c) masComponents_Add(#c, sizeof(c))
#define MAS_COMP(c) #c
#define MAS_COMPONENT_LIST(name, ...)                \
    const char*      name##_List[] = { __VA_ARGS__ };\
    masComponentList name          = { name##_List, (sizeof(name##_List)/sizeof(name##_List[0])) }




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEST
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _test 
{ 

} masPosition, masRotation, masScale, masWorldMatrix, masLocalMatrix, masParent, masChildren;

void w()
{
	MAS_COMPONENT_LIST(TransformCompList,
		MAS_COMP(masPosition),
		MAS_COMP(masRotation),
		MAS_COMP(masScale),
		MAS_COMP(masWorldMatrix),
		MAS_COMP(masLocalMatrix),
		MAS_COMP(masParent),
		MAS_COMP(masChildren));

	masComponentQuery *TransformComps = masComponentRegister_Query(&TransformCompList);
	if (!TransformComps)
		return;
	else
	{
		printf("\n===================================\n");
		printf("QUERY_HASH:  %llu\n", TransformComps->Hash);
		printf("QUERY_COUNT: %llu\n", TransformComps->Count);
		for (size_t c = 0; c < TransformComps->Count; ++c)
		{
			masComponent* Comp = &TransformComps->Components[c];
			printf("    - UNIQUE_ID: %llu\n", Comp->UniqueID);
			printf("    - SIZE:      %llu\n", Comp->Size);
			printf("    - NAME:      %s\n", Comp->Name);
		}
		printf("====================================\n");
	}
}