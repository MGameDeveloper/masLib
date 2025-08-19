

typedef enum masInputUser_
{
    InputUser_1,
    InputUser_2,
    InputUser_3,
    InputUser_4,

    InputUser_Count
} masInputUser;

typedef enum masKeyState_
{
    ButtonState_None,
    ButtonState_Press,
    ButtonState_Release,
    ButtonState_Repeat,
    ButtonState_DoubleClick,

    ButtonState_Count
} masKeyState;

typedef enum masKeyMod_
{
    KeyMod_None   = 0,
    KeyMod_LCtrl  = (1 << 0),
    KeyMod_RCtrl  = (1 << 1),
    KeyMod_LShift = (1 << 2),
    KeyMod_RShift = (1 << 3),
    KeyMod_LAlt   = (1 << 4),
    KeyMod_RAlt   = (1 << 5),
} masKeyMod;

typedef enum masKey_
{
    Key_None,

    Key_Any,

    Key_A, Key_B, Key_C, Key_D, Key_E, Key_F, Key_G, Key_H, Key_I, Key_J, Key_K, Key_L, Key_M, 
    Key_N, Key_O, Key_P, Key_Q, Key_R, Key_S, Key_T, Key_U, Key_V, Key_W, Key_X, Key_Y, Key_Z,

    Key_Numpad0, Key_Numpad1, Key_Numpad2, Key_Numpad3, Key_Numpad4,  
    Key_Numpad5, Key_Numpad6, Key_Numpad7, Key_Numpad8, Key_Numpad9,
      
    Key_Num0, Key_Num1, Key_Num2, Key_Num3, Key_Num4, 
    Key_Num5, Key_Num6, Key_Num7, Key_Num8, Key_Num9,

    Key_F1, Key_F2, Key_F3, Key_F4,  Key_F5,  Key_F6, 
    Key_F7, Key_F8, Key_F9, Key_F10, Key_F11, Key_F12, 

    Key_Up,
    Key_Down,
    Key_Left,
    Key_Right,

    Key_Insert,
    Key_Delete,
    Key_PageUp,
    Key_PageDown,
    Key_End,
    Key_Home,
    Key_CapsLock,
    Key_NumLock,
    Key_Tab,
    Key_Space,
    Key_PrintScreen,
    Key_Escape,
    Key_Tilde,
    Key_Exclamation,
    Key_GraveAccent,
    Key_UnderScore,
    Key_Hyphen,
    Key_Minus,
    Key_Plus,
    Key_Multiply,
    Key_Divide,
    Key_Equal,
    Key_Asterisk,
    Key_Colon,
    Key_Semicolon,
    Key_Apostrophe,
    Key_Comma,
    Key_Quotation,
    Key_Percent,
    Key_Caret,
    Key_AtSign,
    Key_DollarSign,
    Key_Hash,
    Key_Ampersand,
    Key_Backspace,
    Key_Enter,
    Key_LeftParentheses,
    Key_RightParentheses,
    Key_LeftCurlyBraces,
    Key_RightCurlyBraces,
    Key_LeftSquareBrackets,
    Key_RightSquareBrackets,
    Key_Pipe,
    Key_Backslash,
    Key_Forwardslash,
    Key_Colon,
    Key_LessThan,
    Key_GreaterThan,
    Key_Decimal,
    Key_QuestionMark,

    Key_Mouse_Left,
    Key_Mouse_Right,
    Key_Mouse_Middle,
    Key_Mouse_X1,
    Key_Mouse_X2,
    Key_Mouse_WheelUp,
    Key_Mouse_WheelDown,

    Key_Controller_Square,
    Key_Controller_Cross,
    Key_Controller_Circle,
    Key_Controller_Triangle,
    Key_Controller_DpadUp,
    Key_Controller_DpadDown,
    Key_Controller_DpadLeft,
    Key_Controller_DPadRight,
    Key_Controller_Touchpad,
    Key_Controller_L1,
    Key_Controller_L2,
    Key_Controller_L3,
    Key_Controller_R1,
    Key_Controller_R2,
    Key_Controller_R3,
    Key_Controller_Option,
    Key_Controller_Share,
    Key_Controller_LeftAnalogUp,
    Key_Controller_LeftAnalogDown,
    Key_Controller_LeftAnalogLeft,
    Key_Controller_LeftAnalogRight,
    Key_Controller_RightAnalogUp,
    Key_Controller_RightAnalogDown,
    Key_Controller_RightAnalogLeft,
    Key_Controller_RightAnalogRight,

    Key_Count
} masKey;

typedef void(*masInputControllerConnectionCallback)(masInputUser User, bool IsConnected);