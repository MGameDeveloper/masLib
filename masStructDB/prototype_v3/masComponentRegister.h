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
#define MAS_COMPONENT_REGISTER(c) masComponentRegister_Add(#c, sizeof(c))
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

void registeration()
{
	MAS_COMPONENT_REGISTER(masPosition);
	MAS_COMPONENT_REGISTER(masRotation);
	MAS_COMPONENT_REGISTER(masScale);
	MAS_COMPONENT_REGISTER(masWorldMatrix);
	MAS_COMPONENT_REGISTER(masLocalMatrix);
	MAS_COMPONENT_REGISTER(masParent);
	MAS_COMPONENT_REGISTER(masChildren);
}

size_t masEntity_Create()
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

	size_t Entity = masArchtypeRegister_CreateEntity(TransformComps);
	if (Entity == 0)
		printf("[ ERROR ]: Creating Entity\n");
	
	return Entity;
}

size_t masArchtypeRegister_CreateEntity(const masComponentQuery* CompQuery)
{
	masArchtype *Archtype = masArchtype_FindByComponentQuery(CompQuery);
	if (!Archtype)
		Archtype = masArchtype_Create(CompQuery);

	if (!Archtype)
		return 0;

	size_t Entity = masArchtype_CreateEntity(Archtype, CompQuery);
	return Entity;
}

void* masEntity_AddComponent(size_t Entity, const char* CompName)
{
	if (masEntity_HasComponent(Entity, CompName))
		return NULL;

	masArchtype* Archtype = masEntity_GetArchtype(Entity);
	if (!Archtype)
		return NULL;

	const masComponentQuery* CompQuery = masArchtype_GetComponentQuery(Archtype);
	if (!CompQuery)
		return NULL;

	const masComponentQuery* TargetCompQuery = masComponentQuery_Add(CompQuery, CompName); // would add the new compname to the list re order them and calculate the final hash
	if (!TargetCompQuery)
		return NULL;

	masArchtype* TargetArchtype = masArchtype_FindByComponentQuery(TargetCompQuery);
	if (!TargetArchtype)
		TargetArchtype = masArchtype_Create(TargetCompQuery);

	if (!TargetArchtype)
		return NULL;

	if (!masArchtype_MoveEntity(Archtype, Entity, TargetArchtype))
		return NULL;

	void* Comp = masArchtype_GetComponent(Entity, TargetArchtype, CompName);

	return Comp;
}

void parse_comps(const char* comps)
{
	char buf[256];
	strcpy(buf, comps);

	char* token = strtok(buf, ",");
	while (token != NULL)
	{
		while (*token == ' ')
			token++;

		printf("TOKEN: %s\n", token);
		token = strtok(buf, ",");
	}
}