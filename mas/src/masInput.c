#include "masImpl.h"



typedef enum masInputDeviceType_
{
    InputType_Keyboard_Mouse,
    InputType_Controller,

    InputType_Count

} masInputDeviceType;

struct masInputPlayer
{
    masInputDeviceType InputDeviceType;
    masKeyState        Keys[Key_Count];
};