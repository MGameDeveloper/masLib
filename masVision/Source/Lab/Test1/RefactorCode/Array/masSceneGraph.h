#pragma once

#include "masHandle.h"

struct masSceneNode
{
	masHandle Transform;
	masHandle Parent;      // Parent masSceneNode
	masHandle FirstChild;  // First Child as masSceneNode
	masHandle NextSibling; // Next masSceneNode
	masHandle PrevSibling; // Prev masSceneNode
	bool      IsDirty;     // Is this node need an update
};