#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "masWindow.h"

#pragma comment(lib, "glfw3.lib")


static GLFWwindow* GWindow = nullptr;

bool masWindow_Init(const char* Title, int32_t Width, int32_t Height)
{
	if (!glfwInit())
		return false;

	GWindow = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);
	if (!GWindow)
	{
		glfwTerminate();
		return NULL;
	}

	return true;
}

void masWindow_Terminate()
{
	glfwTerminate();
}

bool masWindow_IsClosed()
{
	return glfwWindowShouldClose(GWindow);
}

void masWindow_PeekMessages()
{
	glfwPollEvents();
}

const void* masWindow_NativeHandle()
{
	if (!GWindow)
		return nullptr;
	return glfwGetWin32Window(GWindow);
}

float masWindow_AspectRatio()
{
	int w = 0;
	int h = 0;
	glfwGetWindowSize(GWindow, &w, &h);
	if (h == 0)
		return 0.f;

	return (float(w) / float(h));
}