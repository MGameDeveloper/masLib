#include "masVision.h"

#include "Window/masWindow.h"
#include "Graphics/masGraphics.h"

bool masVision_Init()
{
	uint32_t WindowWidth = 800;
	uint32_t WindowHeight = 600;
	if (!masWindow_Init("masVision", WindowWidth, WindowHeight)) return false;

	const void* WindowHandle = masWindow_NativeHandle();
	if (!masGraphics_Init(WindowHandle, WindowWidth, WindowHeight)) return false;

	//if (!masVision_InitUI())     return false;

	return true;
}

int32_t masVision_Launch()
{
	while (!masWindow_IsClosed())
	{
		masWindow_PeekMessages();

		masGraphics_Render();
	}

	masGraphics_Terminate();
	masWindow_Terminate();

	return 0;
}