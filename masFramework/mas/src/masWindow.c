#include "masImpl.h"

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>


/***************************************************************************************************************************
*
****************************************************************************************************************************/
#define MAS_WINDOW_TITLE_SIZE 32
#define MAS_WINDOW_CLASS_NAME TEXT("masWindowCls")
#define MAS_LOG_ERROR(fmt, ...) printf("[ ERROR ]: "##fmt, __VA_ARGS__)


/***************************************************************************************************************************
*
****************************************************************************************************************************/
typedef struct masWindow_
{
    HWND     Handle;
    wchar_t  Title[MAS_WINDOW_TITLE_SIZE];
    int32_t  PosX;
    int32_t  PosY;
    int32_t  Width;
    int32_t  Height;
    int32_t  DrawAreaWidth;
    int32_t  DrawAreaHeight;
    bool     bClosed;
    bool     bTrackMouse;
} masWindow;

static masWindow Window   = {0};
static HINSTANCE Instance = NULL;


/***************************************************************************************************************************
*
****************************************************************************************************************************/
static LRESULT CALLBACK mas_internal_win32_proc(HWND Hwnd, UINT Msg, WPARAM Wparam, LPARAM Lparam);
static masInputKeyMod   mas_internal_win32_key_mod();
static masInputKey      mas_internal_win32_map_key(int32_t VKCode);
static void             mas_internal_event_add_mouse_button(int32_t VKCode, masInputKeyState KeyState);
static void             mas_internal_event_add_keyboard_key(int32_t VKCode, masInputKeyState KeyState);


/***************************************************************************************************************************
*
****************************************************************************************************************************/
bool mas_impl_window_init(const char* Title, int32_t Width, int32_t Height)
{
    if(!Title || Width <= 0 || Height <= 0)
        return false;
    
	wchar_t WTitle[256] = { 0 };
	int32_t TitleSize = MultiByteToWideChar(CP_UTF8, 0, Title, -1, NULL, 0);
    if(TitleSize <= 0 || TitleSize >= MAS_WINDOW_TITLE_SIZE)
    {
        MAS_LOG_ERROR("Window Title length is either <= 0 or greater than %d\n", MAS_WINDOW_TITLE_SIZE);
        return false;
    }
	MultiByteToWideChar(CP_UTF8, 0, Title, -1, WTitle, TitleSize);

    Instance = GetModuleHandle(NULL);

	WNDCLASSEX wc = { 0 };
	wc.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.hInstance     = Instance;
	wc.lpszClassName = MAS_WINDOW_CLASS_NAME;
	wc.lpfnWndProc   = &mas_internal_win32_proc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = DLGWINDOWEXTRA;
	wc.hIcon         = NULL; //LoadIcon(Instance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hIconSm       = NULL; //LoadIcon(Instance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hCursor       = LoadCursor(Instance, IDC_ARROW);
	wc.lpszMenuName  = NULL;

	if (!RegisterClassEx(&wc))
	{
        MAS_LOG_ERROR("[ WIN32 ]: RegisterClassEx failed\n");
        return false;
    }

	int32_t ScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
	int32_t ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	int32_t PosX         = (ScreenWidth - Width) / 2;
	int32_t PosY         = (ScreenHeight - Height) / 2;

	HWND Handle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, MAS_WINDOW_CLASS_NAME, WTitle, WS_OVERLAPPEDWINDOW,
		PosX, PosY, Width, Height, NULL, NULL, Instance, NULL);
	if (!Handle)
	{
        MAS_LOG_ERROR("[ WIN32 ]: CreateWindowEx failed\n");
		UnregisterClass(MAS_WINDOW_CLASS_NAME, Instance);
		return false;
	}

	RECT Rect = { 0 };
	GetClientRect(Handle, &Rect);
	int32_t DrawAreaWidth  = Rect.right - Rect.left;
	int32_t DrawAreaHeight = Rect.bottom - Rect.top;

	memcpy(Window.Title, WTitle, sizeof(wchar_t) * TitleSize);
    Window.Handle         = Handle;
    Window.PosX           = PosX;
    Window.PosY           = PosY;
    Window.Width          = Width;
    Window.Height         = Height;
    Window.DrawAreaWidth  = DrawAreaWidth;
    Window.DrawAreaHeight = DrawAreaHeight;
    Window.bClosed        = false;

	return true;
}

void mas_impl_window_deinit()
{
    UnregisterClass(MAS_WINDOW_CLASS_NAME, Instance);
}

void* mas_impl_window_handle()
{
    return Window.Handle;
}

const char* mas_impl_window_title()
{
	static char OutTitle[256];
	memset(OutTitle, 0, sizeof(char) * 256);

	int32_t TitleSize = WideCharToMultiByte(CP_UTF8, 0, Window.Title, -1, NULL, 0, NULL, NULL);
    if(TitleSize == 0)
	    return NULL;
	WideCharToMultiByte(CP_UTF8, 0, Window.Title, -1, OutTitle, TitleSize, NULL, NULL);

    return OutTitle;
}

void mas_impl_window_get_pos(int32_t *x, int32_t *y)
{
    if(x)
        *x = Window.PosX;
    if(y)
        *y = Window.PosY;
}

void mas_impl_window_get_size(int32_t *w, int32_t *h)
{
    if(w)
        *w = Window.Width;
    if(h)
        *h = Window.Height;
}

void mas_impl_window_get_draw_area_size(int32_t *w, int32_t *h)
{
    if(w)
        *w = Window.DrawAreaWidth;
    if(h)
        *h = Window.DrawAreaHeight;
}

void mas_impl_window_set_pos(int32_t x, int32_t y)
{
    if(!Window.Handle)
        return;

    Window.PosX = x;
    Window.PosY = y;
    SetWindowPos(Window.Handle, NULL, x, y, Window.Width, Window.Height, SWP_NOZORDER | SWP_NOSIZE); 
}

void mas_impl_window_set_size(int32_t w, int32_t h)
{
    if (!Window.Handle)
	    return;

	Window.Width  = w;
    Window.Height = h;
	SetWindowPos(Window.Handle, NULL, Window.PosX, Window.PosY, w, h, SWP_NOMOVE | SWP_NOZORDER);

	RECT Rect = { 0 };
	GetClientRect(Window.Handle, &Rect);
	Window.DrawAreaWidth  = Rect.right - Rect.left;
	Window.DrawAreaHeight = Rect.bottom - Rect.top;
}

void mas_impl_window_show(bool EnableVisibility)
{
    if (!Window.Handle)
	    return;

    ShowWindow(Window.Handle, (EnableVisibility) ? 1 : 0);
}

bool mas_impl_window_peek_messages()
{
    bool bClosed = Window.bClosed;

    MSG Msg = {0};
    while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
    {
        if(Msg.message == WM_QUIT)
            Window.bClosed = true;
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return bClosed;
}

void mas_impl_window_mouse_enable_capture(bool EnableMouseCapture)
{
    // TODO: 
}

void mas_impl_window_enable_fullscreen(bool EnableFullScreen)
{
    // TODO: 
}

void mas_impl_window_mouse_pos_in_screen(int32_t* x, int32_t* y)
{
    POINT Pos = {0};
    if(GetCursorPos(&Pos))
    {
        if(x)
            *x = Pos.x;
        if(y)
            *y = Pos.y;
    }
}

void mas_impl_window_mouse_pos_in_window(int32_t* x, int32_t* y)
{
    if(!Window.Handle)
        return;

    POINT Pos = {0};
    if(GetCursorPos(&Pos))
    {
        ScreenToClient(Window.Handle, &Pos);
        if(x)
            *x = Pos.x;
        if(y)
            *y = Pos.y;
    }
}

void mas_impl_window_mouse_show(bool EnableVisibility)
{
    // TODO: 
}


/***************************************************************************************************************************
*
****************************************************************************************************************************/
static masInputKeyMod mas_internal_win32_key_mods()
{
	uint8_t KeyMod = 0;
	KeyMod |= (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ? InputKeyMod_LCtrl  : 0;
	KeyMod |= (GetAsyncKeyState(VK_LSHIFT  ) & 0x8000) ? InputKeyMod_LShift : 0;
	KeyMod |= (GetAsyncKeyState(VK_LMENU   ) & 0x8000) ? InputKeyMod_LAlt   : 0;
	KeyMod |= (GetAsyncKeyState(VK_RCONTROL) & 0x8000) ? InputKeyMod_RCtrl  : 0;
	KeyMod |= (GetAsyncKeyState(VK_RSHIFT  ) & 0x8000) ? InputKeyMod_RShift : 0;
	KeyMod |= (GetAsyncKeyState(VK_RMENU   ) & 0x8000) ? InputKeyMod_RAlt   : 0;

    return (masInputKeyMod)KeyMod;
}

static masInputKey mas_internal_win32_map_key(int32_t VKCode)
{
    switch (VKCode)
	{
	case 'A': return InputKey_A;
	case 'B': return InputKey_B;
	case 'C': return InputKey_C;
	case 'D': return InputKey_D;
	case 'E': return InputKey_E;
	case 'F': return InputKey_F;
	case 'G': return InputKey_G;
	case 'H': return InputKey_H;
	case 'I': return InputKey_I;
	case 'J': return InputKey_J;
	case 'K': return InputKey_K;
	case 'L': return InputKey_L;
	case 'M': return InputKey_M;
	case 'N': return InputKey_N;
	case 'O': return InputKey_O;
	case 'P': return InputKey_P;
	case 'Q': return InputKey_Q;
	case 'R': return InputKey_R;
	case 'S': return InputKey_S;
	case 'T': return InputKey_T;
	case 'U': return InputKey_U;
	case 'V': return InputKey_V;
	case 'W': return InputKey_W;
	case 'X': return InputKey_X;
	case 'Y': return InputKey_Y;
	case 'Z': return InputKey_Z;

	case VK_NUMLOCK: return InputKey_NumLock;
	case VK_NUMPAD0: return InputKey_Numpad0;
	case VK_NUMPAD1: return InputKey_Numpad1;
	case VK_NUMPAD2: return InputKey_Numpad2;
	case VK_NUMPAD3: return InputKey_Numpad3;
	case VK_NUMPAD4: return InputKey_Numpad4;
	case VK_NUMPAD5: return InputKey_Numpad5;
	case VK_NUMPAD6: return InputKey_Numpad6;
	case VK_NUMPAD7: return InputKey_Numpad7;
	case VK_NUMPAD8: return InputKey_Numpad8;
	case VK_NUMPAD9: return InputKey_Numpad9;

	case VK_F1:  return InputKey_F1;
	case VK_F2:  return InputKey_F2;
	case VK_F3:  return InputKey_F3;
	case VK_F4:  return InputKey_F4;
	case VK_F5:  return InputKey_F5;
	case VK_F6:  return InputKey_F6;
	case VK_F7:  return InputKey_F7;
	case VK_F8:  return InputKey_F8;
	case VK_F9:  return InputKey_F9;
	case VK_F10: return InputKey_F10;
	case VK_F11: return InputKey_F11;
	case VK_F12: return InputKey_F12;

	case VK_DECIMAL:  return InputKey_Decimal;
	case VK_PRIOR:    return InputKey_PageUp;
	case VK_NEXT:     return InputKey_PageDown;
	case VK_SPACE:    return InputKey_Space;
	case VK_RETURN:   return InputKey_Enter;
	case VK_BACK:     return InputKey_Backspace;
	case VK_TAB:      return InputKey_Tab;
	case VK_SNAPSHOT: return InputKey_PrintScreen;
	case VK_INSERT:   return InputKey_Insert;
	case VK_DELETE:   return InputKey_Delete;
	case VK_DIVIDE:   return InputKey_Divide;
	case VK_MULTIPLY: return InputKey_Multiply;
	case VK_SUBTRACT: return InputKey_Subtract;
	case VK_ADD:      return InputKey_Addition;
	case VK_HOME:     return InputKey_Home;
	case VK_END:      return InputKey_End;
	case VK_ESCAPE:   return InputKey_Escape;
	case VK_CAPITAL:  return InputKey_CapsLock;
	case VK_UP:       return InputKey_Up;
	case VK_DOWN:     return InputKey_Down;
	case VK_LEFT:     return InputKey_Left;
	case VK_RIGHT:    return InputKey_Right;

    case VK_LBUTTON: return InputKey_Mouse_Left;
    case VK_RBUTTON: return InputKey_Mouse_Right;
    case VK_MBUTTON: return InputKey_Mouse_Middle;

	case VK_OEM_102:
	case VK_OEM_NEC_EQUAL:
	case VK_OEM_PLUS:
	case VK_OEM_COMMA:
	case VK_OEM_MINUS:
	case VK_OEM_PERIOD:
	case VK_OEM_1:
	case VK_OEM_2:
	case VK_OEM_3:
	case VK_OEM_4:
	case VK_OEM_5:
	case VK_OEM_6:
	case VK_OEM_7:
	case VK_OEM_8:
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		UINT CharCode = MapVirtualKey(VKCode, MAPVK_VK_TO_CHAR);
		bool bShift = false;
		if ((GetKeyState(VK_SHIFT) & 0x8000))
			bShift = true;
#define MAS_CHAR_KEY(UNSHIFTED, SHIFTED, UNSHIFTED_KEY, SHIFTED_KEY) case UNSHIFTED: if(bShift) return SHIFTED_KEY; else return UNSHIFTED_KEY; break
		switch (CharCode)
		{
			MAS_CHAR_KEY(';', ':',   InputKey_Semicolon,          InputKey_Colon);
			MAS_CHAR_KEY('\'', '\"', InputKey_Apostrophe,         InputKey_Quotation);
			MAS_CHAR_KEY('[', '{',   InputKey_LeftSquareBrackets,  InputKey_LeftCurlyBraces);
			MAS_CHAR_KEY(']', '}',   InputKey_RightSquareBrackets, InputKey_RightCurlyBraces);
			MAS_CHAR_KEY('\\', '|',  InputKey_Backslash,          InputKey_VerticalBar);
			MAS_CHAR_KEY(',', '<',   InputKey_Comma,              InputKey_LessThan);
			MAS_CHAR_KEY('.', '>',   InputKey_Period,             InputKey_GreaterThan);
			MAS_CHAR_KEY('/', '?',   InputKey_Forwardslash,       InputKey_QuestionMark);
			MAS_CHAR_KEY('`', '~',   InputKey_GraveAccent,        InputKey_Tilde);
			MAS_CHAR_KEY('1', '!',   InputKey_Num1,               InputKey_Exclamation);
			MAS_CHAR_KEY('2', '@',   InputKey_Num2,               InputKey_At);
			MAS_CHAR_KEY('3', '#',   InputKey_Num3,               InputKey_Hash);
			MAS_CHAR_KEY('4', '$',   InputKey_Num4,               InputKey_Dollar);
			MAS_CHAR_KEY('5', '%',   InputKey_Num5,               InputKey_Percent);
			MAS_CHAR_KEY('6', '^',   InputKey_Num6,               InputKey_Caret);
			MAS_CHAR_KEY('7', '&',   InputKey_Num7,               InputKey_Ampersand);
			MAS_CHAR_KEY('8', '*',   InputKey_Num8,               InputKey_Asterisk);
			MAS_CHAR_KEY('9', '(',   InputKey_Num9,               InputKey_LeftParenthesis);
			MAS_CHAR_KEY('0', ')',   InputKey_Num0,               InputKey_RightParenthesis);
			MAS_CHAR_KEY('-', '_',   InputKey_Hyphen,             InputKey_UnderScore);
			MAS_CHAR_KEY('=', '+',   InputKey_Equals,             InputKey_Plus);
		}
#undef MAS_CHAR_KEY
	}
	break;
	}

	return InputKey_None;
}

static void mas_internal_event_add_mouse_button(int32_t VKCode, masInputKeyState KeyState)
{
    masInputKey KeyCode = InputKey_None;
    if(HIWORD(VKCode) == XBUTTON1)
        KeyCode = InputKey_Mouse_X1;
    else if(HIWORD(VKCode) == XBUTTON2)
        KeyCode = InputKey_Mouse_X2;
    else
        KeyCode = mas_internal_win32_map_key(VKCode);

    masEvent* Event       = mas_impl_event_add(EventType_Button);
    Event->TimeStamp      = mas_impl_time_now();
    Event->Data.Key.Code  = KeyCode;
    Event->Data.Key.Mod   = mas_internal_win32_key_mods();
    Event->Data.Key.State = KeyState;
}

void mas_internal_event_add_keyboard_key(int32_t VKCode, masInputKeyState KeyState)
{
    masEvent* Event       = mas_impl_event_add(EventType_Button);
    Event->TimeStamp      = mas_impl_time_now();
    Event->Data.Key.Code  = mas_internal_win32_map_key(VKCode);
    Event->Data.Key.Mod   = mas_internal_win32_key_mods();
    Event->Data.Key.State = KeyState;
}

LRESULT CALLBACK mas_internal_win32_proc(HWND Hwnd, UINT Msg, WPARAM Wparam, LPARAM Lparam)
{
    masEvent *Event = NULL;

    switch(Msg)
    {
    case WM_CLOSE:
        DestroyWindow(Hwnd);
        PostQuitMessage(0);
        return 0;

    case WM_DEVICECHANGE: 
        Event = mas_impl_event_add(EventType_Device_Changes);
        break;

    case WM_SIZE: 
    {
       	RECT Rect = { 0 };
		GetWindowRect(Hwnd, &Rect);
		int32_t Width        = Rect.right - Rect.left;
		int32_t Height       = Rect.bottom - Rect.top;
        int32_t ScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
        int32_t ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    }
        return 0;

    case WM_MOVE:
        return 0;

    case WM_MOUSEMOVE:  
        if(!Window.bTrackMouse)
        {
            Window.bTrackMouse = true;

            TRACKMOUSEEVENT TME = { sizeof(TME) };
            TME.dwFlags        = TME_LEAVE;
            TME.hwndTrack      = Hwnd;      
            TrackMouseEvent(&TME);

            Event = mas_impl_event_add(EventType_Mouse_Enter);
        }
        return 0;

    case WM_MOUSELEAVE: 
        Window.bTrackMouse = false;
        Event = mas_impl_event_add(EventType_Mouse_Leave);
        return 0;

    case WM_MOUSEWHEEL:
        {
            Event = mas_impl_event_add(EventType_Button);

            int32_t WheelDelta = GET_WHEEL_DELTA_WPARAM(Wparam);
            if(WheelDelta < 0)
                Event->Data.Key.Code = InputKey_Mouse_WheelDown;
            else
                Event->Data.Key.Code = InputKey_Mouse_WheelUp;
            Event->TimeStamp    = mas_impl_time_now();
            Event->Data.Key.Mod = mas_internal_win32_key_mods();
        } 
        return 0;

    case WM_LBUTTONDOWN:   mas_internal_event_add_mouse_button(VK_LBUTTON, InputKeyState_Press);       return 0;
    case WM_LBUTTONUP:	   mas_internal_event_add_mouse_button(VK_LBUTTON, InputKeyState_Release);     return 0;
    case WM_LBUTTONDBLCLK: mas_internal_event_add_mouse_button(VK_LBUTTON, InputKeyState_DoubleClick); return 0;
    case WM_MBUTTONDOWN:   mas_internal_event_add_mouse_button(VK_MBUTTON, InputKeyState_Press);       return 0;
    case WM_MBUTTONUP:	   mas_internal_event_add_mouse_button(VK_MBUTTON, InputKeyState_Release);     return 0;
    case WM_MBUTTONDBLCLK: mas_internal_event_add_mouse_button(VK_MBUTTON, InputKeyState_DoubleClick); return 0;
    case WM_RBUTTONDOWN:   mas_internal_event_add_mouse_button(VK_RBUTTON, InputKeyState_Press);       return 0;
    case WM_RBUTTONUP:	   mas_internal_event_add_mouse_button(VK_RBUTTON, InputKeyState_Release);     return 0;
    case WM_RBUTTONDBLCLK: mas_internal_event_add_mouse_button(VK_RBUTTON, InputKeyState_DoubleClick); return 0;
    case WM_XBUTTONDOWN:   mas_internal_event_add_mouse_button(Wparam,     InputKeyState_Press);       return 0;
    case WM_XBUTTONUP:     mas_internal_event_add_mouse_button(Wparam,     InputKeyState_Release);     return 0;
    case WM_XBUTTONDBLCLK: mas_internal_event_add_mouse_button(Wparam,     InputKeyState_DoubleClick); return 0;

    case WM_KEYUP:
    case WM_KEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
        {
            WORD VKCode        = LOWORD(Wparam);                          // virtual-key code
			WORD KeyFlags      = HIWORD(Lparam);
			WORD ScanCode      = LOBYTE(KeyFlags);                        // scan code
			BOOL WasKeyDown    = (KeyFlags & KF_REPEAT) == KF_REPEAT;     // previous key-state flag, 1 on autorepeat
			BOOL IsKeyReleased = (KeyFlags & KF_UP) == KF_UP;             // transition-state flag, 1 on keyup

			if (IsKeyReleased)
				mas_internal_event_add_keyboard_key(VKCode, InputKeyState_Release);
			else if (WasKeyDown)
				mas_internal_event_add_keyboard_key(VKCode, InputKeyState_Repeat);
			else
				mas_internal_event_add_keyboard_key(VKCode, InputKeyState_Press);;
        }
        return 0;
    
    case WM_INPUT: // To receive this you need register input device using -> RegisterRawInputDevices
        return 0;

    case WM_INPUT_DEVICE_CHANGE: // To receive this you need to supply RegisterRawInputDevices's struct's flag with RIDEV_DEVNOTIFY
        return 0;

    case WM_CHAR:
    case WM_SYSCHAR:
        Event = mas_impl_event_add(EventType_Text_Enter);
        Event->Data.Unicode = Wparam;
        return 0;
    }

    return DefWindowProc(Hwnd, Msg, Wparam, Lparam);
}
