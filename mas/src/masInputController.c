#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

#include "masImpl.h"


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
#define MAS_REPEAT_INIT_TIME  0.15f
#define MAS_REPEAT_DELAY_TIME 0.3f


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
typedef enum masButton_
{
    Button_Square,
    Button_Cross,
    Button_Circle,
    Button_Triangle,
    Button_DpadUp,
    Button_DpadDown,
    Button_DpadLeft,
    Button_DpadRight,
    Button_Touchpad,
    Button_L1,
    Button_L2,
    Button_L3,
    Button_R1,
    Button_R2,
    Button_R3,
    Button_Option,
    Button_Share,
    Button_LStickUp,
    Button_LStickDown,
    Button_LStickLeft,
    Button_LStickRight,
    Button_RStickUp,
    Button_RStickDown,
    Button_RStickLeft,
    Button_RStickRight,

    Button_Count
} masButton;

typedef struct masController_
{
    struct 
    {
        float LAnalog;
        float RAnalog;
    } Deadzone;
    
    struct 
    {
        float LTrigger;
        float RTrigger;
    } Threshold;

    float ButtonRepeatTime[Button_Count];
    bool  ButtonLastState[Button_Count];
    bool  bConnected;
} masController;


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
static masController Controllers[InputUser_Count] = { 0 };


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
static masInputKey mas_internal_input_controller_map_button(masButton Button)
{
    switch(Button)
    {
    case Button_Square:       return InputKey_Controller_Square; 
    case Button_Cross:        return InputKey_Controller_Cross; 
    case Button_Circle:       return InputKey_Controller_Circle; 
    case Button_Triangle:     return InputKey_Controller_Triangle;
    case Button_DpadUp:       return InputKey_Controller_DpadUp;
    case Button_DpadDown:     return InputKey_Controller_DpadDown;
    case Button_DpadLeft:     return InputKey_Controller_DpadLeft; 
    case Button_DpadRight:    return InputKey_Controller_DpadRight;
    case Button_Touchpad:     return InputKey_Controller_Touchpad,
    case Button_L1:           return InputKey_Controller_L1;
    case Button_L2:           return InputKey_Controller_L2;
    case Button_L3:           return InputKey_Controller_L3;
    case Button_R1:           return InputKey_Controller_R1;
    case Button_R2:           return InputKey_Controller_R2;
    case Button_R3:           return InputKey_Controller_R3;
    case Button_Option        return InputKey_Controller_Option;
    case Button_Share         return InputKey_Controller_Share;
    case Button_LStickUp:     return InputKey_Controller_LStickUp;
    case Button_LStickDown:   return InputKey_Controller_LStickDown;
    case Button_LStickLeft:   return InputKey_Controller_LStickLeft;
    case Button_LStickRight:  return InputKey_Controller_LStickRight;
    case Button_RStickUp:     return InputKey_Controller_RStickUp;
    case Button_RStickDown:   return InputKey_Controller_RStickDown; 
    case Button_RStickLeft:   return InputKey_Controller_RStickLeft; 
    case Button_RStickRight:  return InputKey_Controller_RStickRight;   
    }

    return InputKey_None;
}

static void mas_internal_input_controller_on_analog(masInputUser User, masButton Button, float Value, float Deadzone)
{
    masInputKey Key = mas_internal_input_controller_map_button(Button);
    if(Value > Deadzone || Value < -Deadzone) 
        mas_impl_input_on_axis(User, Key, Value);
    else
        mas_impl_input_on_axis(User, Key, 0.0);
}

static void mas_internal_input_controller_on_trigger(masInputUser User, masButton Button, float Value, float Threshold)
{
    masInputKey Key = mas_internal_input_controller_map_button(Button);
    if(Value > Threshold) 
        mas_impl_input_on_axis(User, Key, Value);
    else
        mas_impl_input_on_axis(User, Key, 0.0);
}

static void mas_internal_event_add_controller_button(masInputUser User, masButton Button, masInputKeyState KeyState)
{
    masEvent* Event = mas_impl_event_add(EventType_Button);
    Event->TimeStamp      = mas_impl_time_now();
    Event->InputUser      = User;
    Event->Data.Key.Code  = mas_internal_input_controller_map_button(Button);
    Event->Data.Key.State = KeyState;
}


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
void mas_impl_input_controller_init()
{
    XINPUT_STATE State = { 0 };
    for(int32_t i = 0; i < InputUser_Count; ++i)
    {
        if(XInputGetState(i, &State) == ERROR_SUCCESS)
        {
            masInputUser User = (masInputUser)i;    
            if(Controllers[i].bConnected == false)
            {
                Controllers[i].bConnected = true;
                mas_impl_input_controller_restore_setting(User);
                printf("[ INFO ]: Controller( %d ) Connected\n", i);
            }
        }
    }
}

void mas_impl_input_controller_deinit()
{

}

void mas_impl_input_controller_check_connection()
{
    XINPUT_STATE State = { 0 };
    for(int32_t i = 0; i < InputUser_Count; ++i)
    {
        if(XInputGetState(i, &State) == ERROR_SUCCESS)
        {
            if(Controllers[i].bConnected == false)
            {
                Controllers[i].bConnected = true;
                printf("[ INFO ]: Controller( %d ) Connected\n", i);
            }
        }
        else
        {
            if(Controllers[i].bConnected == true)
            {
                Controllers[i].bConnected = false;
                printf("[ INFO ]: Controller( %d ) Disconnected\n", i);
            }
        }
    }
}

void mas_impl_input_controller_set_deadzone(masInputUser User, float LAnalog, float RAnalog)
{
    if(User < InputUser_1 || User >= InputUser_Count)
        return;
    Controllers[User].Deadzone.LAnalog = LAnalog;
    Controllers[User].Deadzone.RAnalog = RAnalog;
}

void mas_impl_input_controller_set_threshold(masInputUser User, float LTrigger, float RTrigger)
{
    if(User < InputUser_1 || User >= InputUser_Count)
        return;
    Controllers[User].Threshold.LTrigger = LTrigger;
    Controllers[User].Threshold.RTrigger = RTrigger;
}

void mas_impl_input_controller_restore_setting(masInputUser User)
{
    Controllers[User].Deadzone.LAnalog   = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  / 32768.f;
    Controllers[User].Deadzone.RAnalog   = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE / 32768.f;
    Controllers[User].Threshold.LTrigger = XINPUT_GAMEPAD_TRIGGER_THRESHOLD    / 255.f;
    Controllers[User].Threshold.RTrigger = XINPUT_GAMEPAD_TRIGGER_THRESHOLD    / 255.f;
}

void mas_impl_input_controller_feedback_rumble(masInputUser User, uint16_t LMotorSpeed, uint16_t RMotorSpeed)
{
    XINPUT_VIBRATION Rumble = { 0 };
    Rumble.wLeftMotorSpeed  = LMotorSpeed;
    Rumble.wRightMotorSpeed = RMotorSpeed;
    XInputSetState(User, &Rumble);
}

void mas_impl_input_controller_tick()
{
    XINPUT_STATE    State      = { 0 };
    XINPUT_GAMEPAD *Gamepad    = NULL;
    masController  *Controller = Controllers;

    for(int32_t ControllerIdx = 0; ControllerIdx < InputUser_Count; ++ControllerIdx)
    {
        Controller += ControllerIdx;
        if(!Controller->bConnected)
            continue;

        if(XInputGetState(ControllerIdx, &State) != ERROR_SUCCESS)
        {
            Controller->bConnected = false;
            continue;
        }

        Gamepad = &State.Gamepad;

        static bool Buttons[Button_Count];
        memset(Buttons, 0, sizeof(masButton) * ButtonCount);

        Buttons[Button_Square]       = Gamepad->wButtons & XINPUT_GAMEPAD_X;
		Buttons[Button_Cross]        = Gamepad->wButtons & XINPUT_GAMEPAD_A;
		Buttons[Button_Circle]       = Gamepad->wButtons & XINPUT_GAMEPAD_B;
		Buttons[Button_Triangle]     = Gamepad->wButtons & XINPUT_GAMEPAD_Y;
		Buttons[Button_Option]       = Gamepad->wButtons & XINPUT_GAMEPAD_START;
		Buttons[Button_Share]        = Gamepad->wButtons & XINPUT_GAMEPAD_BACK;
		Buttons[Button_DpadUp]       = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP;
		Buttons[Button_DpadDown]     = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
		Buttons[Button_DpadRight]    = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
		Buttons[Button_DpadLeft]     = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
		Buttons[Button_L1]           = Gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
		Buttons[Button_R1]           = Gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
		Buttons[Button_L3]           = Gamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
		Buttons[Button_R3]           = Gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
		/////////////////////////////////////////////////////////////////////////////////////
		Buttons[Button_L2]           = Gamepad->bLeftTrigger >   Controller->Threshold.LTrigger;
		Buttons[Button_R2]           = Gamepad->bRightTrigger >  Controller->Threshold.RTrigger;
		Buttons[Button_LStickUp]     = Gamepad->sThumbLY >       Controller->Deadzone.LAnalog;
		Buttons[Button_LStickDown]   = Gamepad->sThumbLY <      -Controller->Deadzone.LAnalog;
		Buttons[Button_LStickRight]  = Gamepad->sThumbLX >       Controller->Deadzone.LAnalog;
		Buttons[Button_LStickLeft]   = Gamepad->sThumbLX <      -Controller->Deadzone.LAnalog;
		Buttons[Button_RStickUp]     = Gamepad->sThumbRY >       Controller->Deadzone.RAnalog;
		Buttons[Button_RStickDown]   = Gamepad->sThumbRY <      -Controller->Deadzone.RAnalog;
		Buttons[Button_RStickLeft]   = Gamepad->sThumbRX <      -Controller->Deadzone.RAnalog;
		Buttons[Button_RStickRight]  = Gamepad->sThumbRX >       Controller->Deadzone.RAnalog;


        masInputUser User = (masInputUser)ControllerIdx;
        mas_internal_input_controller_on_trigger(User, Button_L2,          Gamepad->bLeftTrigger  / 255.f,   Controller->Threshold.LTrigger);
        mas_internal_input_controller_on_trigger(User, Button_R2,          Gamepad->bRightTrigger / 255.f,   Controller->Threshold.RTrigger);
        mas_internal_input_controller_on_analog (User, Button_LStickUp,    Gamepad->sThumbLY      / 32767.f, Controller->Deadzone.LAnalog);
        mas_internal_input_controller_on_analog (User, Button_LStickDown,  Gamepad->sThumbLY      / 32768.f, Controller->Deadzone.LAnalog);
        mas_internal_input_controller_on_analog (User, Button_LStickRight, Gamepad->sThumbLX      / 32767.f, Controller->Deadzone.LAnalog);
        mas_internal_input_controller_on_analog (User, Button_LStickLeft,  Gamepad->sThumbLX      / 32768.f, Controller->Deadzone.LAnalog);
        mas_internal_input_controller_on_analog (User, Button_RStickUp,    Gamepad->sThumbRY      / 32767.f, Controller->Deadzone.RAnalog);
        mas_internal_input_controller_on_analog (User, Button_RStickDown,  Gamepad->sThumbRY      / 32768.f, Controller->Deadzone.RAnalog);
        mas_internal_input_controller_on_analog (User, Button_RStickLeft,  Gamepad->sThumbRX      / 32768.f, Controller->Deadzone.RAnalog);
        mas_internal_input_controller_on_analog (User, Button_RStickRight, Gamepad->sThumbRX      / 32767.f, Controller->Deadzone.RAnalog);


        double AppTime = mas_impl_time_now();
        for(int32_t ButtonIdx = 0; ButtonIdx < EButton_Count; ++ButtonIdx)
        {
			bool CurrState  = Buttons[ButtonIdx];
			bool LastState  = Controller->ButtonLastState[ButtonIdx];
			bool IsPressed  = !LastState &&  CurrState;
			bool IsReleased =  LastState && !CurrState;
			bool IsRepeated =  LastState &&  CurrState;

            masButton Button = (masButton)ButtonIdx;
			if (IsRepeated)
			{
				if (Controller->ButtonRepeatTime[ButtonIdx] <= AppTime)
				{
                    mas_internal_event_add_controller_button(User, Button, InputKeyState_Repeat);
					Gamepad->ButtonRepeatTime[ButtonIdx] = AppTime + MAS_REPEAT_DELAY_TIME;
				}
			}
			else if (IsReleased)
				mas_internal_event_add_controller_button(User, Button, InputKeyState_Release);
			else if (IsPressed)
			{
				mas_internal_event_add_controller_button(User, Button, InputKeyState_Press);
				Gamepad->ButtonRepeatTime[ButtonIdx] = AppTime + MAS_REPEAT_INIT_TIME;
			}
        }

        memcpy(Controller->ButtonLastState, Buttons, sizeof(bool) * Button_Count);

        XINPUT_VIBRATION Rumble = { 0 };
        XInputSetState(User, &Rumble);
    }
}