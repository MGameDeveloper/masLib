#pragma once

#include "masCommon.h"

bool masWindow_Init(const char* Title, int32_t Width, int32_t Height);
void masWindow_Terminate();
bool masWindow_IsClosed();
void masWindow_PeekMessages();
const void* masWindow_NativeHandle();
float masWindow_AspectRatio();