#pragma once

class masWindow
{
private:
	struct GLFWwindow *Handle;

	int32_t Width;
	int32_t Height;

public:
	masWindow(int32_t InWidth, int32_t InHeight);
	~masWindow();
	masWindow(const masWindow& Other)            = delete;
	masWindow(masWindow&& Other)                 = delete;
	masWindow& operator=(const masWindow& Other) = delete;
	masWindow& operator=(masWindow&& Other)      = delete;
};

