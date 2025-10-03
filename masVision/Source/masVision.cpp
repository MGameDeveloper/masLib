#include "masVision.h"

#include "Window/masWindow.h"

bool masVision_Init()
{
	if (!masWindow_Init("masVision", 800, 600)) return false;
	//if (!masVision_InitUI())     return false;

	return true;
}

int32_t masVision_Launch()
{
	while (!masWindow_IsClosed())
	{
		masWindow_PeekMessages();
	}

	masWindow_Terminate();

	return 0;
}