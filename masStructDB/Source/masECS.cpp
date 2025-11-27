#include "masECS.h"


bool masECS_Init()
{
	if (!masECSMemory_Init())
		return false;
	if (!masStructDB_Init())
		return false;

	return true;
}

void masECS_DeInit()
{
	masStructDB_DeInit();
	masECSMemory_DeInit();
}

void masECS_Update()
{
	if (!masStructDB_Save())
	{
		// log error or assert
	}
	masECSMemoryFrame_Reset();
}