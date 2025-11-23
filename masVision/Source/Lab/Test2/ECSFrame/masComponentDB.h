#pragma once

#include <stdint.h>

#define MAS_INVALID_VALUE 0xFFFFFFFF

typedef struct _masComponent
{
	uint32_t GUID;       // Generated id for this specific component saved with it on desc when db is written out
	uint32_t Size;       // size of the compoennt at runtime
	uint32_t NameOffset; // Offset into the name buffer
} masComponent;

typedef struct _masComponentName
{ 
	uint8_t  Len;     // len of this name start from Name[1]
	char     Name[1]; // first letter of the name of this component
} masComponentName;

// Name of component would be hashed to be used as index to one these
typedef struct _masComponentHashEntry
{
	uint64_t Hash;       // used to check if collision happenes
	uint32_t DescIdx;    // index into components
} masComponentHashEntry;

typedef struct _masComponentDB
{
	masComponent          *Components;   // component desc meta data
	masComponentName      *Names;   // string buffer and compoent desc has offset into for its name
	masComponentHashEntry *Entries; // hash table entryies array
	uint32_t ComponentNextGUID;     // Used every time a component is add to give it a unique id 
	uint32_t ComponentCapacity;          // max components we can store
	uint32_t ComponentCount;             // current component count
	uint32_t EntryCapacity;         // max hash table entries 
	uint32_t EntryCount;            // current entry count
	uint32_t NameCapacity;          // entire buffer size
	uint32_t NameOffset;            // current offset into buffer can be used to check if any size left in the buffer to resize if needed
};


void masComponentDB_CreateOrLoad();
void masComponentDB_SaveAndDestroy();


typedef struct _masComponentDesc
{
	const char *Name;
	uint32_t    Size;
} masComponentDesc;
#define MAS_COMP(Comp) { #Comp, sizeof(Comp)}

typedef union _masVec2 
{
	float xy[2];
	struct
	{
		float x, y;
	};
} masVec2;



void masComponentDB_Add(masComponentDesc *DescList, uint32_t Count)
{
	uint64_t ComponentListHash = 0;
	for (int32_t i = 0; i < Count; ++i)
	{
		uint64_t CompHash = masComponentDescHash(&Desc[i]);
		uint32_t EntryIdx = CompHash % masComponentDB_HashTableCapacity();

		masComponentHashEntry* Entry = masComponentDB_HashEntry(EntryIdx);
		if (Entry->Hash == CompHash)
		{
			// already added get stored one
		}
		else if (Entry->Hash != CompHash && Entry->Hash != CompHash)
		{
			// collision happens same entry but different compoennts
		}
		else if (Entry->Hash == MAS_INVALID_VALUE)
		{
			// create new component
			// give it new GUID from masComponentDB_NextGUID();
		}
	}
}
