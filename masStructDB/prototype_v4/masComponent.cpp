#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "masComponent.h"
#include "masFrameMemory.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + (offset))
#define MAS_MALLOC(type, size)            (type*)malloc(size)
#define MAS_ENTRY_MAX                     (1024 * 3 * 5)
#define MAS_COMPONENT_MAX                 (1024 * 3)


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
struct masEntry
{
	uint64_t Hash;
	int32_t  Index;
};

struct masComponentDesc
{
	uint32_t  UniqueID;
	uint32_t  Size;
	char      Name[32];
};

struct masComponentMap
{
	masEntry         EntryList[MAS_ENTRY_MAX];
	masComponentDesc ComponentDescList[MAS_COMPONENT_MAX];
	uint32_t         EntryCount;
	uint32_t         ComponentCount;
	uint32_t         UniqueIDGen;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
static masComponentMap* CompMap = NULL;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint64_t masInternal_Hash(const void* InData, uint64_t InSize)
{
	static const uint64_t FNV1A_64_OFFSET_BASIS = 14695981039346656037ull;
	static const uint64_t FNV1A_64_PRIME        = 1099511628211ull;

	const char *Data = (const char*)InData;
	uint64_t    Hash = FNV1A_64_OFFSET_BASIS;
	for (size_t i = 0; i < InSize; ++i) 
	{
		Hash ^= Data[i];
		Hash *= FNV1A_64_PRIME;
	}

	return Hash;
}

static const masComponentDesc* masInternal_FindComponent(const char* Name, uint64_t Hash)
{
	uint32_t          EntryIdx  = Hash % MAS_ENTRY_MAX;
	masEntry         *Entry     = &CompMap->EntryList[EntryIdx];
	masComponentDesc *ComponentDesc = &CompMap->ComponentDescList[Entry->Index];
	if (Entry->Hash == Hash && Entry->Index != -1)
		return ComponentDesc;

	return NULL;
}

const masComponentDesc* masInternal_AddComponent(const char* Name, uint32_t NameLen, uint32_t CompSize, uint64_t NameHash)
{
	if (CompMap->ComponentCount + 1 >= MAS_COMPONENT_MAX)
	{
		printf("[ ERROR ]: No available space in component table for registeration\n");
		return NULL;
	}

	uint32_t          EntryIdx      = NameHash % MAS_ENTRY_MAX;
	masEntry         *Entry         = &CompMap->EntryList[EntryIdx];
	masComponentDesc *ComponentDesc = &CompMap->ComponentDescList[Entry->Index];
	if (Entry->Index != -1)
	{
		printf("[ ERROR ]: Component Name Conflict [ %s with %s ]\n", Name, ComponentDesc->Name);
		return NULL;
	}

	Entry->Hash  = NameHash;
	Entry->Index = CompMap->ComponentCount++;

	ComponentDesc           = &CompMap->ComponentDescList[Entry->Index];
	ComponentDesc->Size     = CompSize;
	ComponentDesc->UniqueID = CompMap->UniqueIDGen++;
	if (NameLen >= 32)
		NameLen = 32 - 1;
	memcpy(ComponentDesc->Name, Name, NameLen);

	printf("[ INFO ]: COMPONENT_REGISTERED: %s\n", ComponentDesc->Name);

	return ComponentDesc;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masComponent_Init()
{
	printf("[ INFO ]: Checking Component File\n");
	printf("[ INFO ]: Component File Doesn't Exists\n");
	printf("[ INFO ]: Creating Component File\n");

	//
	masComponentMap *Map = MAS_MALLOC(masComponentMap, sizeof(masComponentMap));
	if (!Map)
	{
		printf("[ ERROR ]: ALLOCATING_COMPONENT_MAP\n");
		return false;
	}
	memset(Map, 0, sizeof(masComponentMap));
	Map->UniqueIDGen = 1;

	for (int32_t i = 0; i < MAS_ENTRY_MAX; ++i)
		Map->EntryList[i].Index = -1;

	CompMap = Map;

	return true;
}

void masComponent_DeInit()
{
	printf("[ TRACE ]: Writing Components to file\n");
	printf("[ ERROR ]: Failed to write components to file\n");
	
	free(CompMap);
	CompMap = NULL;
}

void masComponent_RegisterByName(const char* Name, uint32_t Size)
{
	if (!CompMap)
	{
		printf("[ ERROR ]: You must call masComponent_Init() before any call to masComponent_* functions\n");
		return;
	}

	uint64_t                NameLen   = strlen(Name);
	uint64_t                NameHash  = masInternal_Hash(Name, NameLen);
	uint64_t                NameHash1 = masInternal_Hash(Name, NameLen);
	const masComponentDesc *FoundComp = masInternal_FindComponent(Name, NameHash);
	if (FoundComp)
	{
		if (strcmp(FoundComp->Name, Name) == 0)
			printf("[ WARNING ]: COMPONENT_ALREADY_REGISTERED[ %s ]\n", Name);
		else
			printf("[ ERROR ]: COMPONENT_REGISTER_CONFLIC[ %s <-> %s ]\n", Name, FoundComp->Name);

		return;
	}

	const masComponentDesc* ComponentDesc = masInternal_AddComponent(Name, NameLen, Size, NameHash);
	if (!ComponentDesc)
		printf("[ ERROR ]: Registering Component [ %s ]\n", Name);
}

masComponent masComponent_Find(const char* Name)
{
	if (!CompMap)
	{
		printf("[ ERROR ]: You must call masComponent_Init() before any call to masComponent_* functions\n");
		return {};
	}

	uint64_t                NameLen       = strlen(Name);
	uint64_t                NameHash      = masInternal_Hash(Name, NameLen);
	uint32_t                EntryIdx      = NameHash % MAS_ENTRY_MAX;
	const masEntry         *Entry         = &CompMap->EntryList[EntryIdx];
	const masComponentDesc *ComponentDesc = &CompMap->ComponentDescList[Entry->Index];
	if (Entry->Hash != NameHash)
	{
		printf("[ ERROR ]: Component: %s NOT_FOUND_MUST_BE_REGISTERED\n", Name);
		return { };
	}

	masComponent Component = { };
	Component.Hash     = Entry->Hash;
	Component.Size     = ComponentDesc->Size;
	Component.UniqueID = ComponentDesc->UniqueID;

	return Component;
}

bool masComponent_IsValid(masComponent Component)
{
	if (Component.Size == 0)
		return false;
	return true;
}


const masComponentList* masComponent_Query(const char* Components)
{
	if (!Components)
		return NULL;

	// Frame scope buffer for temporary processing
	masFrameAllocator* Allocator = masFrameAllocator_Create(1024);
	if (!Allocator)
		return NULL;	

	size_t ComponentNamesLength = strlen(Components);
	if (ComponentNamesLength == 0)
		return NULL;
	ComponentNamesLength++; // null terminaator

	char* ComponentNameBuf = MAS_FRAME_MALLOC(Allocator, char, ComponentNamesLength);
	if (!ComponentNameBuf)
		return NULL;
	memcpy(ComponentNameBuf, Components, ComponentNamesLength - 1);

	// Count Components and Seperate them by inserting \0
	uint32_t ComponentCount = 0;
	char* CompNamePtr = ComponentNameBuf;
	while (*CompNamePtr != NULL)
	{
		ComponentCount++;
		while (*CompNamePtr != NULL)
		{
			if (*CompNamePtr == ',')
			{
				*CompNamePtr = '\0';
			     CompNamePtr++;
				break;
			}
			CompNamePtr++;
		}
	}


	// Create list of pointer where each pointer point to component name in the above buf
	const char** ComponentNameList = MAS_FRAME_MALLOC(Allocator, const char*, ComponentCount * sizeof(const char*));
	if (!ComponentNameList)
		return NULL;

	// Set every pointer to its component
	uint32_t ComponentNameCount = 0;
	CompNamePtr = ComponentNameBuf;
	while (*CompNamePtr != NULL)
	{
		if(*CompNamePtr != '\0' && *CompNamePtr != ' ')
			ComponentNameList[ComponentNameCount++] = CompNamePtr;
		while (*CompNamePtr++ != '\0');
		while (*CompNamePtr == ' ')
			CompNamePtr++;
	}


	// ALLOCATE COMPONENT LIST
	size_t            MemSize       = sizeof(masComponentList) + (sizeof(char) * ComponentNamesLength) + (sizeof(masComponent) * ComponentCount);
	masComponentList *ComponentList = MAS_FRAME_MALLOC(Allocator, masComponentList, MemSize);
	if (!ComponentList)
		return NULL;


	// SETUP COMPONENT LIST
	ComponentList->Names      = MAS_PTR_OFFSET(char,         ComponentList,        sizeof(masComponentList));
	ComponentList->Components = MAS_PTR_OFFSET(masComponent, ComponentList->Names, ComponentNamesLength);
	ComponentList->Count      = ComponentCount;
	ComponentList->NameLen    = ComponentNamesLength;
	memcpy(ComponentList->Names, Components, ComponentList->NameLen);


	// FILL COPMONENT LIST WITH COMPONENT DATA
	for (int32_t i = 0; i < ComponentList->Count; ++i)
	{
		const char* CompName = ComponentNameList[i];
		masComponent SrcComp = masComponent_Find(CompName);

		masComponent *DstComp = &ComponentList->Components[i];
		DstComp->Hash     = SrcComp.Hash;
		DstComp->UniqueID = SrcComp.UniqueID;
		DstComp->Size     = SrcComp.Size;
	}


	// SORT AND HASH COMPONENTS TO WORK AS SIGNITURE FOR THE THE LIST AS MARK FOR AN ARCHTYPE WITH CERTAIN COMPONENTS
	uint32_t* UniqueIDList = MAS_FRAME_MALLOC(Allocator, uint32_t, ComponentList->Count * sizeof(uint32_t));
	for (uint32_t i = 0; i < ComponentList->Count; ++i)
		UniqueIDList[i] = ComponentList->Components[i].UniqueID;
	
	// Sorting is important to have consistent hash value for the same components even if their not in a specific order
	//   sort from the samlled to the largest unique id
	qsort(UniqueIDList, ComponentList->Count, sizeof(uint32_t), 
		[](const void* a, const void* b)->int
		{
			return (*(uint32_t*)a - *(uint32_t*)b);
		});

	ComponentList->Hash = masInternal_Hash(UniqueIDList, sizeof(uint32_t) * ComponentList->Count);

	return ComponentList;
}