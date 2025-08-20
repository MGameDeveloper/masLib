#include "masImpl.h"


/***************************************************************************************************************************
*
****************************************************************************************************************************/
#define MAS_INPUT_DOUBLE_CLICK_THRESHOLD 0.1f


/***************************************************************************************************************************
*
****************************************************************************************************************************/
typedef struct masInputButton_
{
    double           TimeStamp;
    masInputKeyState State;
    masInputKeyMod   Mod;
} masInputButton;

typedef struct masInputPlayer_
{
    masInputButton Buttons[InputKey_Count];
    float          Axes[InputAxis_Count];       
} masInputPlayer;


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
static masInputPlayer InputPlayers[InputUser_Count] = { 0 };


/***************************************************************************************************************************
* 
****************************************************************************************************************************/


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
bool mas_impl_input_init()
{
    mas_impl_input_controller_init();

    return true;
}

void mas_impl_input_deinit()
{
    mas_impl_input_controller_deinit();
}

void mas_impl_input_tick()
{
    mas_impl_input_controller_tick();
}

void mas_impl_input_on_key(masInputUser User, masInputKey Key, masInputKeyState KeyState, masInputKeyMod KeyMod, double TimeStamp)
{
    if(Key < InputKey_Any || Key >= InputKey_Count)
        return;

    masInputPlayer* Player = &InputPlayers[User];
    masInputButton* Button = &Player->Buttons[Key];

    Button->State = KeyState;
    //if((TimeStamp - Button->TimeStamp) >= MAS_INPUT_DOUBLE_CLICK_THRESHOLD)
    //        Button->State = InputKeyState_DoubleClick;
    Button->Mod       = KeyMod;
    Button->TimeStamp = TimeStamp;
}

void mas_impl_input_on_axis(masInputUser User, masInputAxis Axis, float Value)
{
    if(Axis < 0 || Axis >= InputAxis_Count)
        return;
    masInputPlayer *Player = &InputPlayers[User];
    Player->Axes[Axis] = Value;
}

bool mas_impl_input_key_state(masInputUser User, masInputKeyState KeyState, masInputKey Key, masInputKeyMod KeyMod)
{
    masInputPlayer *Player = &InputPlayers[User];
    masInputButton *Button = &Player->Buttons[Key];
    if(Button->State == KeyState && Button->Mod == KeyMod)
        return true;
    return false;
}

float mas_impl_input_axis_value(masInputUser User, masInputAxis Axis)
{
    if(Axis < 0 || Axis >= InputAxis_Count)
        return 0.0f;     
    masInputPlayer *Player = &InputPlayers[User];
    return Player->Axes[Axis];
}

void mas_impl_input_clear()
{
    for(int32_t i = 0; i < InputUser_Count; ++i)
        memset(InputPlayers[i].Buttons, 0, sizeof(masInputButton) * InputKey_Count);
}