#include "masPoolRegistery.h"

#include "masRawPool.h"
#include <stdlib.h>
#include <string.h>


/**************************************************************************************************
*
***************************************************************************************************/
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_DEFAULT_POOL_SIZE 512
#define MAS_INVALID_HANDLE { 0, 0 }


/**************************************************************************************************
*
***************************************************************************************************/
masRawPool::masPoolData* masRawPool::Internal_Create(uint32_t ElementSize, uint32_t Capacity)
{
	uint32_t DataSize = ElementSize * Capacity;
	uint32_t FreeSlotsIDsSize = sizeof(uint32_t) * Capacity;
	uint32_t SlotsSize = sizeof(masSlot) * Capacity;
	uint32_t RequiredMemorySize = sizeof(masPoolData) + DataSize + FreeSlotsIDsSize + SlotsSize;

	masPoolData* Pool = (masPoolData*)malloc(RequiredMemorySize);
	if (!Pool)
		return NULL;
	memset(Pool, 0, RequiredMemorySize);

	Pool->Data        = MAS_PTR_OFFSET(uint8_t,  Pool,        sizeof(masPoolData));
	Pool->Slots       = MAS_PTR_OFFSET(masSlot,  Pool->Data,  DataSize);
	Pool->FreeSlotIDs = MAS_PTR_OFFSET(uint32_t, Pool->Slots, SlotsSize);
	Pool->Capacity    = Capacity;
	Pool->FreeCount   = 0;
	Pool->UsedCount   = 0;
	Pool->AllocIdx    = 0;
	Pool->ElementSize = ElementSize;

	return Pool;
}

bool masRawPool::Internal_Resize()
{
	masPoolData* NewPool = Internal_Create(Pool->ElementSize, Pool->Capacity * 2);
	if (!NewPool)
		return false;

	memcpy(NewPool->Data,        Pool->Data,        Pool->ElementSize * Pool->Capacity);
	memcpy(NewPool->FreeSlotIDs, Pool->FreeSlotIDs, sizeof(uint32_t) * Pool->Capacity);
	memcpy(NewPool->Slots,       Pool->Slots,       sizeof(masSlot) * Pool->Capacity);
	NewPool->AllocIdx  = Pool->AllocIdx;
	NewPool->FreeCount = Pool->FreeCount;
	NewPool->UsedCount = Pool->UsedCount;

	free(Pool);
	Pool = NewPool;

	return true;
}


/**************************************************************************************************
*
***************************************************************************************************/
masRawPool::masRawPool(const char* Name, uint32_t ElementSize) :
	Pool(nullptr)
{
	Pool = Internal_Create(ElementSize, MAS_DEFAULT_POOL_SIZE);
	if (Pool)
		masPoolRegistery::Add(Name, this);
}

masRawPool::~masRawPool()
{
	free(Pool);
	Pool = nullptr;
	masPoolRegistery::Remove(this->ID);
}

void* masRawPool::GetElement(masHandle Handle)
{
	if (!IsValidHandle(Handle))
		return nullptr;

	masSlot *Slot    = &Pool->Slots[Handle.SlotIdx];
	void    *Element = MAS_PTR_OFFSET(void, Pool->Data, Pool->ElementSize * Slot->DataIdx);

	return Element;
}

masHandle masRawPool::Alloc()
{
	if (!Pool)
		return masHandle();

	masSlot     *Slot   = NULL;
	uint32_t     SlotID = 0;

	if (Pool->AllocIdx < Pool->Capacity)
	{
		SlotID = Pool->AllocIdx++;
		Slot = &Pool->Slots[SlotID];
		Slot->DataIdx = SlotID;
	}

	if (!Slot && Pool->FreeCount > 0)
	{
		SlotID = Pool->FreeSlotIDs[--Pool->FreeCount];
		Slot = &Pool->Slots[SlotID];
	}

	if (!Slot)
	{
		if (!Internal_Resize())
			return masHandle();

		SlotID = Pool->AllocIdx++;
		Slot   = &Pool->Slots[Pool->AllocIdx++];
		Slot->DataIdx = SlotID;
	}

	Slot->GenID++;
	Slot->RefCount++;

	masHandle Handle = { };
	Handle.SlotIdx = SlotID;
	Handle.GenID   = Slot->GenID;

	Pool->UsedCount++;

	return Handle;
}

void masRawPool::Free(masHandle& Handle)
{
	if (!IsValidHandle(Handle))
		return;

	masSlot* Slot = &Pool->Slots[Handle.SlotIdx];
	Slot->RefCount--;
	if (Slot->RefCount == 0)
	{
		Slot->GenID++;
		Pool->FreeSlotIDs[Pool->FreeCount++] = Handle.SlotIdx;
	}

	Handle.SlotIdx = 0;
	Handle.GenID   = 0;
}

bool masRawPool::IsValidHandle(const masHandle& Handle)
{
	if (!Pool || (Handle.SlotIdx == 0 && Handle.GenID == 0) || (Handle.SlotIdx >= Pool->Capacity))
		return false;
	return (Handle.GenID == Pool->Slots[Handle.SlotIdx].GenID);
}
