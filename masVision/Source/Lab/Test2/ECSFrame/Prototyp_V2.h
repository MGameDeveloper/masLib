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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

masEntity masArchType_CreateEntity(masArchType* ArchType) 
{ 
	if (!ArchType)
		return { 0 };

	return { 0 }; 
}

typedef struct _masCompInfo
{
	const char* Name;
	uint64_t    Size;
} masCompInfo;

masArchType* masArchType_FindOrCreate(masCompInfo* CompInfoList, uint32_t Count)
{
	//
}

#define MAS_COMP(Comp) {#Comp, sizeof(Comp)}
#define MAS_ARCHTYPE_GET(VarName, ...)\
    masArchType *VarName = NULL;\
    {\
        masCompInfo VarName##List[] = { __VA_ARGS__ }; \
        uint32_t    VarName##Count = sizeof(VarName##List) / sizeof(VarName##List[0]); \
		VarName = masArchType_FindOrCreate(VarName##List, VarName##Count);\
    }

masEntity masEntity_Create()
{
	// Persistent Components in order for any entity to be in the scene
	MAS_ARCHTYPE_GET(ArchType,
		MAS_COMP(masSceneNode),
		MAS_COMP(masPosition),
		MAS_COMP(masEuler),
		MAS_COMP(masScale),
		MAS_COMP(masMatrix));

	// Its up to archtype internal code to either create from existing chunk or allocate new chunk and add
	return masArchType_CreateEntity(ArchType);
}

void masEntity_Attach(masEntity Parent, masEntity Child)
{
	// 
}

void masEntity_AddComponents(masEntity Entity, masCompInfo* CompList, uint32_t CompCount)
{
	//
}

#define MAS_ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))
#define MAS_ENTITY_ADD_COMPONENTS(Entity, ...)\
    masCompInfo Entity##CompList[] = {__VA_ARGS__};\
    uint32_t    Entity##CompCount  = MAS_ARRAY_SIZE(Entity##CompList);\
    masEntity_AddComponents(Entity, Entity##CompList, Entity##CompCount)


#define MAS_DEFINE_TAG_COMPONENT(Name) typedef struct _Name {} Name

MAS_DEFINE_TAG_COMPONENT(masPlayerTag);
MAS_DEFINE_TAG_COMPONENT(masEnemy_Nightmare);
MAS_DEFINE_TAG_COMPONENT(masEnemy_Goblin);
MAS_DEFINE_TAG_COMPONENT(masHideElixer);
MAS_DEFINE_TAG_COMPONENT(masExoticWeaponDrop);
MAS_DEFINE_TAG_COMPONENT(masFinalLordQuestObjective);
MAS_DEFINE_TAG_COMPONENT(masHideElixerUnlock);
MAS_DEFINE_TAG_COMPONENT(masPlayerSlave);

typedef struct _masStaticMesh
{
	uint32_t Verices;
	uint32_t Indices;
	uint32_t IndexCount;
} masStaticMesh;

void Init()
{
	masEntity PlayerEnt = masEntity_Create();
	MAS_ENTITY_ADD_COMPONENTS(PlayerEnt,
		MAS_COMP(masPlayerTag),
		MAS_COMP(masStaticMesh));

	masEntity EnemyEnt = masEntity_Create();
	MAS_ENTITY_ADD_COMPONENTS(EnemyEnt,
		MAS_COMP(masEnemy_Nightmare),
		MAS_COMP(masEnemy_Goblin),
		MAS_COMP(masHideElixer),
		MAS_COMP(masExoticWeaponDrop),
		MAS_COMP(masFinalLordQuestObjective),
		MAS_COMP(masHideElixerUnlock));

	// Upon Defeat become slave to the player
	MAS_ENTITY_ADD_COMPONENTS(EnemyEnt, MAS_COMP(masPlayerSlave));
	masEntity_Attach(PlayerEnt, EnemyEnt);

	// suppose we imported assimp scene and done the necessary parsing 

	masEntity AK47 = masEntity_Create();
	for (int32_t i = 0; i < MeshCount; ++i)
	{
		masEntity WeaponPart = masEntity_Create();
		MAS_ENTITY_ADD_COMPONENTS(WeaponPart, MAS_COMP(masStaticMesh));

		masStaticMesh* Mesh = masEntity_GetComponent(MAS_COMP(masStaticMesh));
		if (Mesh)
		{
			masMeshAsset *Asset = masLoadMesh(AK47AssimScene, i);
			Mesh->Verices    = Asset->VerticesHandle;
			Mesh->Indices    = Asset->IndicesHandle;
			Mesh->IndexCount = Asset->IndexCount;
			Mesh->Name       = Asset->NameHandle;
			Mesh->Size       = Asset->Size;
			Mesh->Material   = Asset->DefaultMaterial;
		}
		masEntity_Attach(AK47, WeaponPart);
	}

	masEntity PlayerMainWeaponSocket = masEntity_Create();
	masEntity_Attach(PlayerEnt, PlayerMainWeaponSocket);
	masEntity_Attach(PlayerMainWeaponSocket, AK47);
}