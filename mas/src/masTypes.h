#pragma once 

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>


/***************************************************************************************************************************
* MACROS: 
****************************************************************************************************************************/
#ifdef MAS_UNICODE
    #define UNICODE
    #define _UNICODE
#endif

#ifdef MAS_UNICODE
    typedef wchar_t masChar;
    #define MAS_TEXT(s) L##s
#else
    typedef char masChar;
    #define MAS_TEXT(s) s
#endif 


/***************************************************************************************************************************
* DIRECTORY: 
****************************************************************************************************************************/
typedef struct _masFileBuf masFileBuf;
typedef struct _masFile
{
    masChar *Path;
    masChar *Name;
    masChar *Extension;
    uint32_t PathSize;
    uint32_t NameSize;
    uint32_t ExtensionSize;
    uint32_t Size;
} masFile;


/***************************************************************************************************************************
* INPUT:
****************************************************************************************************************************/
typedef enum masInputUser_
{
    InputUser_1,
    InputUser_2,
    InputUser_3,
    InputUser_4,

    InputUser_Count
} masInputUser;

typedef enum masInputKeyState_
{
    InputKeyState_None,
    InputKeyState_Press,
    InputKeyState_Release,
    InputKeyState_Repeat,
    InputKeyState_DoubleClick,

    InputKeyState_Count
} masInputKeyState;

typedef enum masInputKeyMod_
{
    InputKeyMod_None   = 0,
    InputKeyMod_LCtrl  = (1 << 0),
    InputKeyMod_RCtrl  = (1 << 1),
    InputKeyMod_LShift = (1 << 2),
    InputKeyMod_RShift = (1 << 3),
    InputKeyMod_LAlt   = (1 << 4),
    InputKeyMod_RAlt   = (1 << 5),
} masInputKeyMod;

typedef enum masInputKey_
{
    InputKey_None = -1,

    InputKey_Any,

    InputKey_A, InputKey_B, InputKey_C, InputKey_D, InputKey_E, InputKey_F, InputKey_G, InputKey_H, InputKey_I, InputKey_J, InputKey_K, InputKey_L, InputKey_M, 
    InputKey_N, InputKey_O, InputKey_P, InputKey_Q, InputKey_R, InputKey_S, InputKey_T, InputKey_U, InputKey_V, InputKey_W, InputKey_X, InputKey_Y, InputKey_Z,

    InputKey_Numpad0, InputKey_Numpad1, InputKey_Numpad2, InputKey_Numpad3, InputKey_Numpad4,  
    InputKey_Numpad5, InputKey_Numpad6, InputKey_Numpad7, InputKey_Numpad8, InputKey_Numpad9,
      
    InputKey_Num0, InputKey_Num1, InputKey_Num2, InputKey_Num3, InputKey_Num4, 
    InputKey_Num5, InputKey_Num6, InputKey_Num7, InputKey_Num8, InputKey_Num9,

    InputKey_F1, InputKey_F2, InputKey_F3, InputKey_F4,  InputKey_F5,  InputKey_F6, 
    InputKey_F7, InputKey_F8, InputKey_F9, InputKey_F10, InputKey_F11, InputKey_F12, 

    InputKey_Up,
    InputKey_Down,
    InputKey_Left,
    InputKey_Right,

    InputKey_Insert,
    InputKey_Delete,
    InputKey_PageUp,
    InputKey_PageDown,
    InputKey_End,
    InputKey_Home,
    InputKey_CapsLock,
    InputKey_NumLock,
    InputKey_Tab,
    InputKey_Space,
    InputKey_PrintScreen,
    InputKey_Escape,
    InputKey_Tilde,
    InputKey_Exclamation,
    InputKey_GraveAccent,
    InputKey_UnderScore,
    InputKey_Hyphen,
    InputKey_Minus,
    InputKey_Plus,
    InputKey_Multiply,
	InputKey_Subtract,
	InputKey_Addition,
    InputKey_Divide,
    InputKey_Equals,
    InputKey_Asterisk,
    InputKey_Colon,
    InputKey_Semicolon,
    InputKey_Apostrophe,
    InputKey_Comma,
    InputKey_Quotation,
    InputKey_Percent,
    InputKey_Caret,
    InputKey_AtSign,
    InputKey_DollarSign,
    InputKey_Hash,
    InputKey_Ampersand,
    InputKey_Backspace,
    InputKey_Enter,
    InputKey_LeftParenthesis,
    InputKey_RightParenthesis,
    InputKey_LeftCurlyBraces,
    InputKey_RightCurlyBraces,
    InputKey_LeftSquareBrackets,
    InputKey_RightSquareBrackets,
    InputKey_VerticalBar,
    InputKey_Backslash,
    InputKey_Forwardslash,
    InputKey_LessThan,
    InputKey_GreaterThan,
    InputKey_Decimal,
    InputKey_QuestionMark,
    InputKey_Period,
    InputKey_At,
    InputKey_Dollar,
    
    InputKey_Mouse_Left,
    InputKey_Mouse_Right,
    InputKey_Mouse_Middle,
    InputKey_Mouse_X1,
    InputKey_Mouse_X2,
    InputKey_Mouse_WheelUp,
    InputKey_Mouse_WheelDown,

    InputKey_Controller_Square,
    InputKey_Controller_Cross,
    InputKey_Controller_Circle,
    InputKey_Controller_Triangle,
    InputKey_Controller_DpadUp,
    InputKey_Controller_DpadDown,
    InputKey_Controller_DpadLeft,
    InputKey_Controller_DpadRight,
    InputKey_Controller_Touchpad,
    InputKey_Controller_L1,
    InputKey_Controller_L2,
    InputKey_Controller_L3,
    InputKey_Controller_R1,
    InputKey_Controller_R2,
    InputKey_Controller_R3,
    InputKey_Controller_Option,
    InputKey_Controller_Share,
    InputKey_Controller_LStickUp,
    InputKey_Controller_LStickDown,
    InputKey_Controller_LStickLeft,
    InputKey_Controller_LStickRight,
    InputKey_Controller_RStickUp,
    InputKey_Controller_RStickDown,
    InputKey_Controller_RStickLeft,
    InputKey_Controller_RStickRight,

    InputKey_Count

} masInputKey;

typedef enum masInputAxis_
{
    InputAxis_None = -1,

    InputAxis_Controller_L2,
    InputAxis_Controller_R2,
    InputAxis_Controller_LStickX,
    InputAxis_Controller_LStickY,
    InputAxis_Controller_RStickX,
    InputAxis_Controller_RStickY,

    InputAxis_Count

} masInputAxis;

typedef void(*masInputControllerConnectionCallback)(masInputUser User, bool IsConnected);
