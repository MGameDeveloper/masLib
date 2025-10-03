#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "masWindow.h"

#pragma comment(lib, "glfw3.lib")


masWindow::masWindow(int32_t InWidth, int32_t InHeight) : 
	Handle(nullptr), 
	Width(InWidth), 
	Height(InHeight)
{
	Handle = glfwCreateWindow(Width, Height, "masVision", nullptr, nullptr);
}

masWindow::~masWindow()
{
	if (Handle)
	{
		glfwDestroyWindow(Handle);
	}
}