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
	masHandle Parent; // handles into scenegraph mapper
	masHandle Prev;	  // handles into scenegraph mapper
	masHandle Next;	  // handles into scenegraph mapper
	masHandle ChildHead;  // handles into scenegraph mapper
	masHandle ChildTail;
	int32_t   ChildCount;
	bool      IsDirty;
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

masNode* masInternal_GetNode(masSceneGraph* SceneGraph, masHandle NodeHandle)
{
	if (!SceneGraph || NodeHandle.Signiture == 0)
		return NULL;
	if (SceneGraph->RegisterID != NodeHandle.PoolID)
		return NULL;

	masMapper* Mapper = &SceneGraph->Mappers[NodeHandle.MapperIdx];
	if (Mapper->Version != NodeHandle.Version)
		return NULL;

	masNode* Node = &SceneGraph->Nodes[Mapper->DataIdx];
	return Node;
}

masHandle masInternal_Alloc(masSceneGraph** SceneGraphPtr)
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
			SceneGraph->FreeIndices[SceneGraph->FreeCount] = -1;
		}
	}

	if (MapperIdx == -1)
		MapperIdx = SceneGraph->AllocIdx++;

	masMapper* Mapper = &SceneGraph->Mappers[MapperIdx];
	Mapper->DataIdx = MapperIdx;
	Mapper->RefCount = 1;
	Mapper->Version++;
	if (Mapper->Version == 0)
		Mapper->Version = 1;

	masHandle Handle = { 0 };
	Handle.MapperIdx = MapperIdx;
	Handle.PoolID = SceneGraph->RegisterID;
	Handle.Version = Mapper->Version;

	return Handle;
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
			SceneGraph->FreeIndices[SceneGraph->FreeCount] = -1;
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
	Node->Parent    = { 0 };
	Node->Prev      = { 0 };
	Node->Next      = { 0 };
	Node->ChildHead = { 0 };
	Node->ChildTail = { 0 };

	return Handle;
}

void masSceneGraph_Free(masSceneGraph* SceneGraph, masHandle* Handle)
{
	if (!SceneGraph || !Handle || Handle->Signiture == 0)
		return;
	if (SceneGraph->RegisterID != Handle->PoolID)
		return;

	masMapper* ParentMapper = &SceneGraph->Mappers[Handle->MapperIdx];
	if (ParentMapper->Version != Handle->Version)
		return;

	ParentMapper->RefCount--;
	if (ParentMapper->RefCount <= 0)
	{
		// free all children and dettach from prev next if exist and free it
	}

	*Handle = { 0 };
}


/************************************************************************************************************/

masHandle masSceneGraph_AddChildFirst(masSceneGraph** SceneGraphPtr, masHandle ParentHandle)
{
	if (!SceneGraphPtr || ParentHandle.Signiture == 0)
		return { 0 };

	masSceneGraph* SceneGraph = *SceneGraphPtr;
	if (SceneGraph->RegisterID != ParentHandle.PoolID)
		return { 0 };

	masNode* ParentNode = masInternal_GetNode(SceneGraph, ParentHandle);
	if (!ParentNode)
		return { 0 };

	masHandle NewChildHandle = masInternal_Alloc(SceneGraphPtr);
	if (NewChildHandle.Signiture == 0)
		return { 0 };

	masNode* NewChildNode   = masInternal_GetNode(SceneGraph, NewChildHandle);
	NewChildNode->Transform = { 0 }; // need to create transform component and return handle for this node
	NewChildNode->Parent    = ParentHandle;
	NewChildNode->Next      = { 0 };
	NewChildNode->Prev      = { 0 };
	NewChildNode->ChildHead = { 0 };
	NewChildNode->ChildTail = { 0 };
	NewChildNode->IsDirty   = false;

	masHandle  HeadChildHandle = ParentNode->ChildHead;
	masNode   *HeadChildNode   = masInternal_GetNode(SceneGraph, HeadChildHandle);
	if (HeadChildNode)
	{
		ParentNode->ChildHead  = NewChildHandle;
		NewChildNode->Next     = HeadChildHandle;
		HeadChildNode->Prev    = NewChildHandle;
		ParentNode->ChildCount++;
	}
	else
	{
		ParentNode->ChildHead = NewChildHandle;
		ParentNode->ChildTail = NewChildHandle;
		ParentNode->ChildCount = 1;
	}

	
	return NewChildHandle;
}

masHandle masSceneGraph_AddChildLast(masSceneGraph** SceneGraphPtr, masHandle ParentHandle)
{
	if (!SceneGraphPtr || ParentHandle.Signiture == 0)
		return { 0 };

	masSceneGraph* SceneGraph = *SceneGraphPtr;
	if (SceneGraph->RegisterID != ParentHandle.PoolID)
		return { 0 };

	masNode* ParentNode = masInternal_GetNode(SceneGraph, ParentHandle);
	if (!ParentNode)
		return { 0 };

	masHandle NewChildHandle = masInternal_Alloc(SceneGraphPtr);
	if (NewChildHandle.Signiture == 0)
		return { 0 };

	masNode* NewChildNode   = masInternal_GetNode(SceneGraph, NewChildHandle);
	NewChildNode->Transform = { 0 }; // need to create transform component and return handle for this node
	NewChildNode->Parent    = ParentHandle;
	NewChildNode->Next      = { 0 };
	NewChildNode->Prev      = { 0 };
	NewChildNode->ChildHead = { 0 };
	NewChildNode->ChildTail = { 0 };
	NewChildNode->IsDirty   = false;

	masHandle  ChildTailHandle = ParentNode->ChildTail;
	masNode   *ChildTailNode   = masInternal_GetNode(SceneGraph, ChildTailHandle);
	if (ChildTailNode)
	{
		ChildTailNode->Next   = NewChildHandle;
		NewChildNode->Prev    = ChildTailHandle;
		ParentNode->ChildTail = NewChildHandle;
		ParentNode->ChildCount++;
	}
	else
	{
		ParentNode->ChildHead = NewChildHandle;
		ParentNode->ChildTail = NewChildHandle;
		ParentNode->ChildCount = 1;
	}

	return NewChildHandle;
}