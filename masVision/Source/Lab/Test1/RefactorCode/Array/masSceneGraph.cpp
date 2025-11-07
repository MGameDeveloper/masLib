#include "masSceneGraph.h"

#include <stdlib.h>
#include <string.h>


/*************************************************************************************************************
*
**************************************************************************************************************/
#define MAS_REGISTER_NAME_SIZE 128
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)


/*************************************************************************************************************
*
**************************************************************************************************************/
struct masMapper
{
	uint32_t DataIdx;
	uint16_t Version;
	uint16_t RefCount;
};

struct masNode
{
	masHandle Transform;
	masHandle Parent;
	masHandle Prev;
	masHandle Next;
	int32_t   ChildIdx;
	int32_t   ChildCount;
};

struct masSceneGraph
{
	masNode   *Nodes;
	masMapper *Mappers;
	int32_t   *FreeIndices;
	int32_t    FreeCount;
	int32_t    AllocIdx;
	int32_t    Capacity;
	int32_t    UseCount;
	int32_t    ResizeCounter;
	int8_t     RegisterID;
	char       RegisterName[MAS_REGISTER_NAME_SIZE];
};


/*************************************************************************************************************
*
**************************************************************************************************************/
bool masInternal_Resize(masSceneGraph** SceneGraphPtr)
{
	if (!SceneGraphPtr || !(*SceneGraphPtr))
		return false;

	masSceneGraph* SceneGraph = *SceneGraphPtr;
	int32_t        Capacity   = SceneGraph->Capacity * 2;

	uint64_t  NodesSize   = sizeof(masNode)   * Capacity;
	uint64_t  MappersSize = sizeof(masMapper) * Capacity;
	uint64_t  IndicesSize = sizeof(int32_t)   * Capacity;
	uint64_t  MemorySize  = sizeof(masSceneGraph) + NodesSize + IndicesSize;
	masSceneGraph* LocalSceneGraph  = (masSceneGraph*)malloc(MemorySize);
	if (!LocalSceneGraph)
		return false;

	memset(LocalSceneGraph, 0, MemorySize);

	LocalSceneGraph->Nodes       = MAS_PTR_OFFSET(masNode,   LocalSceneGraph,          sizeof(masSceneGraph));
	LocalSceneGraph->Mappers     = MAS_PTR_OFFSET(masMapper, LocalSceneGraph->Nodes,   NodesSize);
	LocalSceneGraph->FreeIndices = MAS_PTR_OFFSET(int32_t,   LocalSceneGraph->Mappers, MappersSize);
	LocalSceneGraph->Capacity    = Capacity;
	LocalSceneGraph->AllocIdx    = SceneGraph->AllocIdx;
	LocalSceneGraph->FreeCount   = SceneGraph->FreeCount;
	LocalSceneGraph->UseCount    = SceneGraph->UseCount;
	LocalSceneGraph->RegisterID  = SceneGraph->RegisterID;

	memcpy(LocalSceneGraph->Nodes,        SceneGraph->Nodes,        sizeof(masNode)   * SceneGraph->Capacity);
	memcpy(LocalSceneGraph->Mappers,      SceneGraph->Mappers,      sizeof(masMapper) * SceneGraph->Capacity);
	memcpy(LocalSceneGraph->FreeIndices,  SceneGraph->FreeIndices,  sizeof(int32_t)   * SceneGraph->Capacity);
	memcpy(LocalSceneGraph->RegisterName, SceneGraph->RegisterName, strlen(SceneGraph->RegisterName));

	free(*SceneGraphPtr);
	*SceneGraphPtr = LocalSceneGraph;

	// masArrayRegistery_Update(LocalArray->RegisteredID, LocalArray);

	return true;
}


/*************************************************************************************************************
*
**************************************************************************************************************/
masSceneGraph* masSceneGraph_Create(const char* RegisterName, int32_t Capacity)
{
	uint64_t NodesSize       = sizeof(masNode)   * Capacity;
	uint64_t MappersSize     = sizeof(masMapper) * Capacity;
	uint64_t FreeIndicesSize = sizeof(int32_t)   * Capacity;
	uint64_t MemorySize      = sizeof(masSceneGraph) + NodesSize + MappersSize + FreeIndicesSize;
	masSceneGraph* SceneGraph = (masSceneGraph*)malloc(MemorySize);
	if (!SceneGraph)
		return NULL;
	memset(SceneGraph, 0, MemorySize);

	SceneGraph->Nodes       = MAS_PTR_OFFSET(masNode,   SceneGraph,          sizeof(masSceneGraph));
	SceneGraph->Mappers     = MAS_PTR_OFFSET(masMapper, SceneGraph->Nodes,   NodesSize);
	SceneGraph->FreeIndices = MAS_PTR_OFFSET(int32_t,   SceneGraph->Mappers, MappersSize);
	SceneGraph->Capacity    = Capacity;
	SceneGraph->AllocIdx    = 0;
	SceneGraph->FreeCount   = 0;
	SceneGraph->UseCount    = 0;
	SceneGraph->RegisterID  = -1;

	if (RegisterName)
	{
		uint64_t NameLen = strlen(RegisterName);
		if (NameLen >= MAS_REGISTER_NAME_SIZE)
			NameLen = MAS_REGISTER_NAME_SIZE - 1;
		memcpy(SceneGraph->RegisterName, RegisterName, NameLen);

		//Array->RegisterID = masArrayRegistery_Add(RegisterName, Array);
	}

	return SceneGraph;
}

void masSceneGraph_Destroy(masSceneGraph** SceneGraphPtr)
{
	if (!SceneGraphPtr || !(*SceneGraphPtr))
		return;

	// TODO:
}

int32_t masSceneGraph_Capacity(masSceneGraph* SceneGraph)
{
	if (!SceneGraph)
		return 0;
	return SceneGraph->Capacity;
}
int32_t masSceneGraph_Size(masSceneGraph* SceneGraph)
{
	if (!SceneGraph)
		return 0;
	return SceneGraph->UseCount;
}
int32_t masSceneGraph_RegisterID(masSceneGraph* SceneGraph)
{
	if (!SceneGraph)
		return -1;
	return SceneGraph->RegisterID;
}
int32_t masSceneGraph_ResizeCount(masSceneGraph* SceneGraph)
{
	if (!SceneGraph)
		return 0;
	return SceneGraph->ResizeCounter;
}
int32_t masSceneGraph_FreeCount(masSceneGraph* SceneGraph)
{
	if (!SceneGraph)
		return 0;
	return SceneGraph->FreeCount;
}
const char* masSceneGraph_RegsiterName(masSceneGraph* SceneGraph)
{
	if (!SceneGraph)
		return NULL;
	return SceneGraph->RegisterName;
}

masHandle masSceneGraph_Alloc(masSceneGraph** SceneGraphPtr)
{
	if (!SceneGraphPtr || !(*SceneGraphPtr))
		return { 0 };

	masSceneGraph* SceneGraph = *SceneGraphPtr;

	int32_t MapperIdx = -1;
	if (SceneGraph->AllocIdx >= SceneGraph->Capacity)
	{
		if (SceneGraph->FreeCount <= 0)
		{
			if (masInternal_Resize(SceneGraphPtr))
				SceneGraph = *SceneGraphPtr;
			else
				return { 0 };
		}
		else
		{
			MapperIdx = SceneGraph->FreeIndices[--SceneGraph->FreeCount];
			SceneGraph->FreeIndices[SceneGraph->FreeCount + 1] = -1;
		}
	}

	if (MapperIdx == -1)
		MapperIdx = SceneGraph->AllocIdx++;

	masMapper* Mapper = &SceneGraph->Mappers[MapperIdx];
	if (Mapper->Version == 0)
		Mapper->Version = 1;
	Mapper->RefCount = 1;
	Mapper->DataIdx = MapperIdx; // CAUTION: data memory my be compacted to ensure its contigous upon processing so mappers indices to data should have specific logic to avoid bugs

	masHandle Handle = { 0 };
	Handle.PoolID    = SceneGraph->RegisterID;
	Handle.MapperIdx = MapperIdx;
	Handle.Version   = Mapper->Version;

	SceneGraph->UseCount++;

	masNode* Node = &SceneGraph->Nodes[Mapper->DataIdx];
	//Node->Transform = masTransform_Create();
	Node->Parent     = { 0 };
	Node->Prev       = { 0 };
	Node->Next       = { 0 };
	Node->ChildIdx   = -1;
	Node->ChildCount = 0;

	return Handle;
}

void masSceneGraph_Free(masSceneGraph* SceneGraph, masHandle* Handle)
{
	// TODO
}

bool masSceneGraph_AllocChildren(masSceneGraph* SceneGraph, masHandle ParentNode, int32_t ChildrenCount)
{

}
