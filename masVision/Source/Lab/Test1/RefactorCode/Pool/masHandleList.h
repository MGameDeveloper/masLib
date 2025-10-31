#pragma once

#include "masHandle.h"

class masHandleList
{
private:
	masHandleNode *Nodes;
	uint32_t      *FreeIndices;
	uint32_t       FreeCount;
	uint32_t       UsedCount;
	uint32_t       Capacity;
	
public:
	masHandleList();
	~masHandleList();
	masHandleList(const masHandleList& Other)          = delete;
	masHandleList(masHandleList&& Other)               = delete;
	masHandleList& operator=(const masHandleList& Rhs) = delete;
	masHandleList& operator=(masHandleList& Rhs)       = delete;

	// rest of api here 
};