#pragma once

#include <stdint.h>


///////////////////////////////////////////////////////////////////////////////////////
// Entity Handle and a way to prevent using stale handle by checking generation
//     from Mapper against handle's
///////////////////////////////////////////////////////////////////////////////////////
typedef union _masEntity
{
	uint64_t Handle;
	struct
	{
		uint32_t MapperIndex;
		uint16_t Generation;
		uint16_t UnUsed;
	};
} masEntity;

typedef struct _masEntityMapper
{
	uint16_t Generation;
	uint32_t ArchType;
	uint16_t Chunk;
	uint16_t IndexInChunk;
} masEntityMapper;


///////////////////////////////////////////////////////////////////////////////////////
// Component info to be specified by every archtype according to its unique combination
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masComponentInfo
{
	uint32_t ID;     // to the component description table
	uint32_t Offset; // offet inside the archtype
	uint16_t Size;
	uint16_t Alignment;
} masComponentInfo;



///////////////////////////////////////////////////////////////////////////////////////
// Each archtype represents specific combination of components and all entities with same
//     components will live in the archtype's chunks where each chunk represents 16KB page of entities
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masArchTypeChunk
{
	uint8_t  *Page;
	uint16_t  PageID;
	uint16_t  EntityCount;
} masArchTypeChunk;

typedef struct _masArchType
{
	masArchTypeChunk *ChunkList;
	masComponentInfo *ComponentInfoList;
	uint16_t          MaxEntityPerChunk;
	uint16_t          ChunkListCapacity;
	uint16_t          ChunkListCount;
	uint16_t          ComponentCount;
} masArchType;


///////////////////////////////////////////////////////////////////////////////////////
// Represents all archtypes created and facilitate search an archtype with a specific
//     components combination
///////////////////////////////////////////////////////////////////////////////////////
typedef struct _masArchTypeRegistery
{
	masArchType *ArchTypeList;
	// masHashTable HashTable; // maps component combination hash into index to the archtypelist
	uint32_t     Capacity;
	uint32_t     Count;
} masArchTypeRegistery;


///////////////////////////////////////////////////////////////////////////////////////
// Persistent Components for entity to be placed in the scene 
//     -> [ masPositioni masScale, masEuler, masMatrix, masSceneNode ]
// # This is upon using the ecs to build the initial frame work
///////////////////////////////////////////////////////////////////////////////////////
typedef union _masVec3
{
	float xyz[3];
	struct
	{
		float x, y, z;
	};
} masVec3, masPosition, masScale, masEuler;

typedef union _masVec4
{
	float xyzw[4];
	struct
	{
		float x, y, z, w;
	};
} masVec4, masQuaternion;

typedef union _masMatrix
{
	float m16[16];
} masMatrix;

typedef struct _masSceneNode
{
	masEntity Parent;
	masEntity Child;
	masEntity Next;
	masEntity Prev;
} masSceneNode;




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Component Registery Prototype
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _masComponent
{
	uint16_t Size;
	char     Name[64];
} masComponent;

typedef struct _masComponentRegistery
{
	masComponent *Components;
	uint32_t      Capacity;
	uint16_t      UsedCount;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void masComponent_RegisterA(uint32_t CompSize, const char* CompName)
{
	static uint32_t ComponentGUID = 1;
	// hash compname
	// access hash table using hash
	// check if hashes matches
	// already registered ignore/notify user to remove the duplicate registeration call
	// not matches register since it would trigger error is used without being registered
}

#define MAS_COMPONENT_REGISTERA(Component) masComponent_RegisterA(sizeof(Component), #Component)
void masRegisterCoreComponentsA()
{
	MAS_COMPONENT_REGISTERA(masVec3);
	MAS_COMPONENT_REGISTERA(masPosition);
	MAS_COMPONENT_REGISTERA(masScale);
	MAS_COMPONENT_REGISTERA(masEuler);
	MAS_COMPONENT_REGISTERA(masVec4);
	MAS_COMPONENT_REGISTERA(masQuaternion);
	MAS_COMPONENT_REGISTERA(masMatrix);
	MAS_COMPONENT_REGISTERA(masSceneNode);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// B
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _masComponent
{
	uint32_t ID;
	uint32_t Size;
	char     Name[32];
} masComponent;

static masComponent Components[masCoreComponentCount] = { };
static uint32_t     LastCompIDGenerated = 0;


#define MAS_COMPONENT_ENUM(Component) Component##ID
typedef enum _masCoreComponentIDs
{
	MAS_COMPONENT_ENUM(masVec3),
    MAS_COMPONENT_ENUM(masPosition),
    MAS_COMPONENT_ENUM(masScale),
    MAS_COMPONENT_ENUM(masEuler),
    MAS_COMPONENT_ENUM(masVec4),
    MAS_COMPONENT_ENUM(masQuaternion),
    MAS_COMPONENT_ENUM(masMatrix),
    MAS_COMPONENT_ENUM(masSceneNode),

	masCoreComponentCount
} masCoreComponentIDs;

void masComponent_Register(uint32_t ComponentID, uint32_t ComponentSize, const char* ComponentName)
{
	if (ComponentID < LastCompIDGenerated)
	{
		masComponent* Comp = &Components[ComponentID];
		if (Comp->ID != ComponentID)
		{
			Comp->ID   = ComponentID;
			Comp->Size = ComponentSize;
			
			uint32_t NameLen = strlen(ComponentName);
			if (NameLen >= 32)
				NameLen = 32 - 1;
			memcpy(Comp->Name, ComponentName, NameLen);
		}
		else
		{
			// raise warning and notify user of duplication registering
		}
	}
}

#define MAS_COMPONENT_REGISTER(Component) masComponent_Register(MAS_COMPONENT_ENUM(Component), sizeof(Component), #Component)
void masRegisterCoreComponents(uint32_t ComponentID, uint32_t ComponentSize, const char* ComponentName)
{
	MAS_COMPONENT_REGISTER(masVec3);
	MAS_COMPONENT_REGISTER(masPosition);
	MAS_COMPONENT_REGISTER(masScale);
	MAS_COMPONENT_REGISTER(masEuler);
	MAS_COMPONENT_REGISTER(masVec4);
	MAS_COMPONENT_REGISTER(masQuaternion);
	MAS_COMPONENT_REGISTER(masMatrix);
	MAS_COMPONENT_REGISTER(masSceneNode);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _masComponent
{
	uint32_t ID;
	uint32_t NameOffset;
	uint16_t Size;
	uint8_t  NameLen;
} masComponent;

typedef struct _masComponentEntry
{
	uint64_t Hash;
	uint32_t Idx;
} masComponentEntry;

typedef struct _masComponentFileHeader
{
	uint32_t Tag;
	uint32_t Version;
	uint32_t ComponentListOffset;
	uint32_t ComponentHashTableOffset;
	uint32_t ComponentNameListOffset;
	uint32_t HashTableSize;
	uint32_t ComponentListSize;
	uint32_t ComponentCount;
	uint32_t ComponentGUID; // got increased every time we add to the db
} masComponentFileHeader;

typedef struct _masComponentDB
{
	FILE* FileHandle;
	masComponentFileHeader GCompnentFileHeader;
} masComponentDB;

static masComponentDB GComponentDB = { }; // upon running create file if not exist or open one when already there


typedef struct _masComponentDesc
{
	const char* Name;
	uint32_t Size;
	uint32_t ID; // got its data from the db 
} masComponentDesc;

void masEntity_AddComponents(masEntity Entity, masComponentDesc* CompDescList, uint32_t Count)
{
	uint64_t ArchTypeHash = 0;
	for (int32_t i = 0; i < Count; ++i)
	{
		const char* CompName = CompDescList[i].Name;

		// hash it to uint64_t
		// check if it already added in GComponentDB File
		// if yes get ID from there along with its metadata
		// if not and mapped to already occupied slot resize DB and rehash all components name since names are db this would be easy
		// if not and mapped to valid slot add along with required data and assign a new id for it based on db ComponentGUID from header
		// acculamulate ids to calculate archtypehash
	}


	// find archtype by hash
	// if not found create one with componentdesclist 
	// add entity to it
	// if found and entity lives in different archtype copy it to its new archtype
}


#define MAS_COMP(Component) {#Component, sizeof(Component)}
#define MAS_ENTITY_ADD_COMPONENTS(Entity, ...)\
    masComponentDesc Entity##CompList[] = { __VA_ARGS__ };\
    uint32_t Entity##CompCount = sizeof(Entity##CompList) / sizeof(Entity##CompList[0]);\
    masEntity_AddComponents(Entity, Entity##CompList, Entity##CompCount)

masEntity masEntity_Create()
{
	masEntity Entity = masEntity_NewHandle(); // just to lay the prototype, but later this would get handle to sparse structure that prevent from stale handles
	MAS_ENTITY_ADD_COMPONENTS(Entity,
		MAS_COMP(masPosition),
		MAS_COMP(masEuler),
		MAS_COMP(masScale),
		MAS_COMP(masMatrix),
		MAS_COMP(masSceneNode));
}

void masEntity_Attach(masEntity Parent, masEntity Child)
{
	// code
}

typedef struct _masPlayer 
{
	// data
} masPlayer;

typedef struct _masStaticMesh
{
	// data
} masStaticMesh;

typedef struct _masCamera
{
	// data
} masCamera;

typedef struct _masSpringArm
{
	// data
} masSpringArm;

void TestPrototype()
{
	masEntity PlayerMeshEnt = masEntity_Create();
	MAS_ENTITY_ADD_COMPONENTS(PlayerMeshEnt, MAS_COMP(masStaticMesh));

	masEntity Player = masEntity_Create();
	MAS_ENTITY_ADD_COMPONENTS(Player,
		MAS_COMP(masPlayer),
		MAS_COMP(masCamera),
		MAS_COMP(masSpringArm));

	masEntity_Attach(Player, PlayerMeshEnt);
}