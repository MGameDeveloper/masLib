#include "masVision.h"

#include "Window/masWindow.h"
#include "Graphics/masGraphics.h"
#include "Time/masTime.h"

#include "Lab/Test1/Test1.h"

static LabAPI* Test = NULL;

bool masVision_Init()
{
	masTime_Init();

	uint32_t WindowWidth = 800;
	uint32_t WindowHeight = 600;
	if (!masWindow_Init("masVision", WindowWidth, WindowHeight)) return false;

	const void* WindowHandle = masWindow_NativeHandle();
	if (!masGraphics_Init(WindowHandle, WindowWidth, WindowHeight)) return false;

	//if (!masVision_InitUI())     return false;

	Test = new Test1;
	if (!Test->Init())
		return false;

	return true;
}

int32_t masVision_Launch()
{
	while (!masWindow_IsClosed())
	{
		masTime_Tick();

		masWindow_PeekMessages();
		
		if (Test)
		{
			Test->Tick();
			Test->Present();
		}

		//masGraphics_Render();
	}

	if (Test)
	{
		Test->Terminate();
		delete Test;
	}

	masGraphics_Terminate();
	masWindow_Terminate();

	return 0;
}