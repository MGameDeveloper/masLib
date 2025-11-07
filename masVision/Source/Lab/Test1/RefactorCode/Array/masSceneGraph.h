#pragma once

#include "masHandle.h"


struct masSceneGraph;

masSceneGraph* masSceneGraph_Create(const char* RegisterName, int32_t Capacity);
void           masSceneGraph_Destroy(masSceneGraph** SceneGraphPtr);
int32_t        masSceneGraph_Capacity(masSceneGraph* SceneGraph);
int32_t        masSceneGraph_Size(masSceneGraph* SceneGraph);
int32_t        masSceneGraph_RegisterID(masSceneGraph* SceneGraph);
int32_t        masSceneGraph_ResizeCount(masSceneGraph* SceneGraph);
int32_t        masSceneGraph_FreeCount(masSceneGraph* SceneGraph);
const char*    masSceneGraph_RegsiterName(masSceneGraph* SceneGraph);
masHandle      masSceneGraph_Alloc(masSceneGraph** SceneGraphPtr);
void           masSceneGraph_Free(masSceneGraph* SceneGraph, masHandle* Handle);



