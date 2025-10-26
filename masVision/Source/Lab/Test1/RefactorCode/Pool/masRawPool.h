#pragma once

#include "masHandle.h"

class masRawPool
{
private:
	struct masSlot
	{
		uint32_t DataIdx; // Zero is valid
		uint32_t GenID;   // Zero is invalid
		uint32_t RefCount;
	};

	struct masPoolData
	{
		uint8_t  *Data;
		uint32_t *FreeSlotIDs;
		masSlot  *Slots;       // TODO: CORRECTLY SET DATA IDX
		uint32_t  Capacity;
		uint32_t  FreeCount;
		uint32_t  UsedCount;
		uint32_t  AllocIdx;
		uint32_t  ElementSize;
	};

	masPoolData* Pool;
	uint8_t      ID;

private:
	masPoolData* Internal_Create(uint32_t ElementSize, uint32_t Capacity);
	bool Internal_Resize();

public:
	masRawPool(const char* Name, uint32_t ElementSize);
	~masRawPool();
	masRawPool(const masRawPool& Other)            = delete; // Copy Semantic
	masRawPool& operator=(const masRawPool& Other) = delete; // Copy Semantic
	masRawPool(masRawPool&& Other)                 = delete; // Move Semantic
	masRawPool& operator=(masRawPool&& Other)      = delete; // Move Semantic

	void*     GetElement(masHandle Handle);
	masHandle Alloc();
	void      Free(masHandle& Handle);
	bool      IsValidHandle(const masHandle& Handle);
};

/*
* TODO: ITERATOR TO ITERATE OVER ALL DATA
*         - USEFUL FOR REGISTERY SYSTEM TO ITERATE OVER ITEMS
*/