#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include <string.h>
#include <stdio.h>

#include "masInternal.h"


/***************************************************************************************************************************
*
****************************************************************************************************************************/
#define MAS_WINDOW_TITLE_SIZE 32
#define MAS_WINDOW_CLASS_NAME "masWindowCls"
#define MAS_LOG_ERROR(fmt, ...) printf("[ ERROR ]: "##fmt, __VA_ARGS__)


/***************************************************************************************************************************
*
****************************************************************************************************************************/
typedef struct masWindow_
{
    HWND     Handle;
    char     Title[MAS_WINDOW_TITLE_SIZE];
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
static masKeyMod        mas_internal_win32_key_mod();
static masKey           mas_internal_win32_map_key(int32_t VKCode);
static void             mas_internal_event_add_mouse_button(int32_t VKCode, masKeyState KeyState);
static void             mas_internal_event_add_keyboard_key(int32_t VKCode, masKeyState KeyState);


/***************************************************************************************************************************
*
****************************************************************************************************************************/
bool mas_impl_window_init(const char* Title, int32_t Width, int32_t Height)
{
    if(!Title || Width <= 0 || Height <= 0)
        return false;
    
    int32_t TitleSize = strlen(Title);
    if(TitleSize <= 0 || TitleSize >= MAS_WINDOW_TITLE_SIZE)
    {
        MAS_LOG_ERROR("Window Title length is either <= 0 or greater than %d\n", MAS_WINDOW_TITLE_SIZE);
        return false;
    }

    Instance = GetModuleHandle(NULL);

	WNDCLASSEX wc = { 0 };
	wc.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.hInstance     = Instance;
	wc.lpszClassName = WND_CLASSNAME;
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
	int32_t PosX         = { (ScreenWidth - Width) / 2, (ScreenHeight - Height) / 2 };
	int32_t PosY         = { (ScreenWidth - Width) / 2, (ScreenHeight - Height) / 2 };

	HWND Handle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, MAS_WINDOW_CLASS_NAME, Title, WS_OVERLAPPEDWINDOW,
		PosX, PosY, Width, Height, NULL, NULL, Instance, NULL);
	if (!Handle)
	{
        MAS_LOG_ERROR("[ WIN32 ]: CreateWindowEx failed\n");
		UnregisterClassA(MAS_WINDOW_CLASS_NAME, Instance);
		return false;
	}

	RECT Rect = {};
	GetClientRect(Handle, &Rect);
	int32_t DrawAreaWidth  = { Rect.right - Rect.left, Rect.bottom - Rect.top };
	int32_t DrawAreaHeight = { Rect.right - Rect.left, Rect.bottom - Rect.top };

	memcpy(masWindow->Title, Title, sizeof(char) ** TitleSize);
    Window->Handle         = Handle;
    Window->PosX           = PosX;
    Window->PosY           = PosY;
    Window->Width          = Width;
    Window->Height         = Height;
    Window->DrawAreaWidth  = DrawAreaWidth;
    Window->DrawAreaHeight = DrawAreaHeight;
    Window->bClose         = false;

	return true;
}

void mas_impl_window_deinit()
{
    UnregisterClassA(MAS_WINDOW_CLASS_NAME, Window.Handle);
}

void* mas_impl_window_handle()
{
    return Window.Handle;
}

const char* mas_impl_window_title()
{
    return Window.Title;
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
    SetWindowPos(Window->Handle, NULL, x, y, Window.Width, Window.Height, SWP_NOZORDER | SWP_NOSIZE); 
}

void mas_impl_window_set_size(int32_t w, int32_t h)
{
    if (!Window.Handle)
	    return;

	Window.Width  = w;
    Window.Height = h;
	SetWindowPos(Window.Handle, NULL, Wnd->PosX, Wnd->PosY, w, h, SWP_NOMOVE | SWP_NOZORDER);

	RECT Rect = {};
	GetClientRect(Window.Handle, &Rect);
	Wnd->DrawAreaWidth  = Rect.right - Rect.left;
	Wnd->DrawAreaHeight = Rect.bottom - Rect.top;
}

void mas_impl_window_show(bool EnableVisibility)
{
    if (!Window.Handle)
	    return;

    ShowWindow(Wnd->Handle, (EnableVisibility) ? 1 : 0);
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

/***************************************************************************************************************************
*
****************************************************************************************************************************/
static masKeyMod mas_internal_win32_key_mod()
{
	uint8_t KeyMod = 0;
	KeyMod |= (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ? KeyMod_LCtrl  : 0;
	KeyMod |= (GetAsyncKeyState(VK_LSHIFT  ) & 0x8000) ? KeyMod_LShift : 0;
	KeyMod |= (GetAsyncKeyState(VK_LMENU   ) & 0x8000) ? KeyMod_LAlt   : 0;
	KeyMod |= (GetAsyncKeyState(VK_RCONTROL) & 0x8000) ? KeyMod_RCtrl  : 0;
	KeyMod |= (GetAsyncKeyState(VK_RSHIFT  ) & 0x8000) ? KeyMod_RShift : 0;
	KeyMod |= (GetAsyncKeyState(VK_RMENU   ) & 0x8000) ? KeyMod_RAlt   : 0;

    return (masKeyMod)KeyMod;
}

static masKey mas_internal_win32_map_key(int32_t VKCode)
{
    switch (VKCode)
	{
	case 'A': return Key_A;
	case 'B': return Key_B;
	case 'C': return Key_C;
	case 'D': return Key_D;
	case 'E': return Key_E;
	case 'F': return Key_F;
	case 'G': return Key_G;
	case 'H': return Key_H;
	case 'I': return Key_I;
	case 'J': return Key_J;
	case 'K': return Key_K;
	case 'L': return Key_L;
	case 'M': return Key_M;
	case 'N': return Key_N;
	case 'O': return Key_O;
	case 'P': return Key_P;
	case 'Q': return Key_Q;
	case 'R': return Key_R;
	case 'S': return Key_S;
	case 'T': return Key_T;
	case 'U': return Key_U;
	case 'V': return Key_V;
	case 'W': return Key_W;
	case 'X': return Key_X;
	case 'Y': return Key_Y;
	case 'Z': return Key_Z;

	case VK_NUMLOCK: return Key_NumLock;
	case VK_NUMPAD0: return Key_Numpad0;
	case VK_NUMPAD1: return Key_Numpad1;
	case VK_NUMPAD2: return Key_Numpad2;
	case VK_NUMPAD3: return Key_Numpad3;
	case VK_NUMPAD4: return Key_Numpad4;
	case VK_NUMPAD5: return Key_Numpad5;
	case VK_NUMPAD6: return Key_Numpad6;
	case VK_NUMPAD7: return Key_Numpad7;
	case VK_NUMPAD8: return Key_Numpad8;
	case VK_NUMPAD9: return Key_Numpad9;

	case VK_F1:  return Key_F1;
	case VK_F2:  return Key_F2;
	case VK_F3:  return Key_F3;
	case VK_F4:  return Key_F4;
	case VK_F5:  return Key_F5;
	case VK_F6:  return Key_F6;
	case VK_F7:  return Key_F7;
	case VK_F8:  return Key_F8;
	case VK_F9:  return Key_F9;
	case VK_F10: return Key_F10;
	case VK_F11: return Key_F11;
	case VK_F12: return Key_F12;

	case VK_DECIMAL:  return Key_Decimal;
	case VK_PRIOR:    return Key_PageUp;
	case VK_NEXT:     return Key_PageDown;
	case VK_SPACE:    return Key_Space;
	case VK_RETURN:   return Key_Enter;
	case VK_BACK:     return Key_Backspace;
	case VK_TAB:      return Key_Tab;
	case VK_SNAPSHOT: return Key_PrintScreen;
	case VK_INSERT:   return Key_Insert;
	case VK_DELETE:   return Key_Delete;
	case VK_DIVIDE:   return Key_Divide;
	case VK_MULTIPLY: return Key_Multipy;
	case VK_SUBTRACT: return Key_Subtract;
	case VK_ADD:      return Key_Addition;
	case VK_HOME:     return Key_Home;
	case VK_END:      return Key_End;
	case VK_ESCAPE:   return Key_Escape;
	case VK_CAPITAL:  return Key_CapsLock;
	case VK_UP:       return Key_Up;
	case VK_DOWN:     return Key_Down;
	case VK_LEFT:     return Key_Left;
	case VK_RIGHT:    return Key_Right;

	//case VK_LCONTROL: return Key_LCtrl;
	//case VK_LSHIFT:	  return Key_LShift;
	//case VK_LMENU:	  return Key_LAlt;
	//case VK_RCONTROL: return Key_RCtrl;
	//case VK_RSHIFT:	  return Key_RShift;
	//case VK_RMENU:	  return Key_RAlt;

    case VK_LBUTTON: return Key_Mouse_Left;
    case VK_RBUTTON: return Key_Mouse_Right;
    case VK_MBUTTON: return Key_Mouse_Middle;

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
			MAS_CHAR_KEY(';', ':',   Key_SemiColon,          Key_Colon);
			MAS_CHAR_KEY('\'', '\"', Key_Apostrophe,         Key_DoubleQuote);
			MAS_CHAR_KEY('[', '{',   Key_LeftSquareBracket,  Key_LeftCurlyBrace);
			MAS_CHAR_KEY(']', '}',   Key_RightSquareBracket, Key_RightCurlyBrace);
			MAS_CHAR_KEY('\\', '|',  Key_BackSlash,          Key_VerticalBar);
			MAS_CHAR_KEY(',', '<',   Key_Comma,              Key_LessThan);
			MAS_CHAR_KEY('.', '>',   Key_Period,             Key_GreaterThan);
			MAS_CHAR_KEY('/', '?',   Key_ForwardSlash,       Key_QuestionMark);
			MAS_CHAR_KEY('`', '~',   Key_GraveAccent,        Key_Tilde);
			MAS_CHAR_KEY('1', '!',   Key_Num1,               Key_ExclamationMark);
			MAS_CHAR_KEY('2', '@',   Key_Num2,               Key_At);
			MAS_CHAR_KEY('3', '#',   Key_Num3,               Key_Hash);
			MAS_CHAR_KEY('4', '$',   Key_Num4,               Key_Dollar);
			MAS_CHAR_KEY('5', '%',   Key_Num5,               Key_Percent);
			MAS_CHAR_KEY('6', '^',   Key_Num6,               Key_Caret);
			MAS_CHAR_KEY('7', '&',   Key_Num7,               Key_Ampersand);
			MAS_CHAR_KEY('8', '*',   Key_Num8,               Key_Asterisk);
			MAS_CHAR_KEY('9', '(',   Key_Num9,               Key_LeftParenthesis);
			MAS_CHAR_KEY('0', ')',   Key_Num0,               Key_RightParenthesis);
			MAS_CHAR_KEY('-', '_',   Key_Hyphen,             Key_UnderScore);
			MAS_CHAR_KEY('=', '+',   Key_Equals,             Key_Plus);
		}
#undef MAS_CHAR_KEY
	}
	break;
	}

	return Key_None;
}

static void mas_internal_event_add_mouse_button(int32_t VKCode, masKeyState KeyState)
{
    masKey KeyCode = Key_None;
    if(HIWORD(VKCode) == XBUTTON1)
        KeyCode = Key_Mouse_X1;
    else if(HIWORD(VKCode) == XBUTTON2)
        KeyCode = Key_Mouse_X2;
    else
        KeyCode = mas_internal_win32_map_key(VKCode);

    masEvent* Event       = mas_impl_event_add(EventType_Mouse_Button);
    Event->TimeStamp      = mas_impl_time_now();
    Event->Data.Key.Code  = KeyCode;
    Event->Data.Key.Mod   = mas_internal_win32_key_mod();
    Event->Data.Key.State = KeyState;
}

void mas_internal_event_add_keyboard_key(int32_t VKCode, masKeyState KeyState)
{
    masEvent* Event       = mas_impl_event_add(EventType_Keyboard_Button);
    Event->TimeStamp      = mas_impl_time_now();
    Event->Data.Key.Code  = mas_internal_win32_map_key(VKCode);
    Event->Data.Key.Mod   = mas_internal_win32_key_mod();
    Event->Data.Key.State = KeyState;
}

LRESULT CALLBACK mas_internal_win32_proc(HWND Hwnd, UINT Msg, WPARAM Wparam, LPARAM Lparam)
{
    masEvent Event = {0};

    switch(Msg)
    {
    case WM_CLOSE:
        DestroyWindow(Hwnd);
        PostQuitMessage(0);
        Event = mas_impl_event_add(EventType_Window_Close);
        return 0;

    case WM_DEVICECHANGE: 
        Event = mas_impl_event_add(EventType_Device_Changes);
        break;

    case WM_SIZE: 
    {
       	RECT Rect = {};
		GetWindowRect(Wnd->Handle, &Rect);
		int32_t Width        = Rect.right - Rect.left;
		int32_t Height       = Rect.bottom - Rect.top;
        int32_t ScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
        int32_t ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

        int32_t EventWidth  = 0;
        int32_t EventHeight = 0;
		switch (wparam)
		{
		case SIZE_MAXIMIZED: 
            EventWidth = ScreenWidth; 
            EventHeight = ScreenHeight; 
            break;

		case SIZE_MINIMIZED: 
            EventWidth = 0; 
            EventHeight = 0; 
            break;

		case SIZE_RESTORED:  
            EventWidth = Width; 
            EventHeight = Height; 
            break;
		} 

        Event = mas_impl_event_add(EventType_Window_Resize);
        Event->Data.Size = { EventWidth, EventHeight };
    }
        return 0;

    case WM_MOVE:
        Event            = mas_impl_event_add(EventType_Window_Move);
        Event->Data.Size = { GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam) };
        return 0;

    case WM_MOUSEMOVE:  
        if(!Window.bTrackMouse)
        {
            Window.bTrackMouse = true;

            TRACKMOUSEEVENT TME = { sizeof(TME) };
            TME.dwFlags        = TME_LEAVE;
            TME.hwndTrack      = Hwnd;      
            TrackMouseEvent(&TME);

            //if(!Window.bMouseInside)
            //{
            //    Window.bMouseInside = true;
                Event = mas_impl_event_add(EventType_Mouse_Enter);
            //}

        }

        Event           = mas_impl_event_add(EventType_Mouse_Move);
        Event->Data.Pos = { GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam) };
        return 0;

    case WM_MOUSELEAVE: 
        Window.bTrackMouse = false;
        //Window.bMouseInsid = false;
        Event = mas_impl_event_add(EventType_Mouse_Leave);
        return 0;

    case WM_MOUSEWHEEL:
        {
            Event = mas_impl_event_add(EventType_Mouse_Wheel);

            int32_t WheelDelta = GET_WHEEL_DELTA_WPARAM(Wparam);
            if(WheelDelta < 0)
                Event->Data.MosueWheel.Direction = Key_Mouse_WheelDown;
            else
                Event->Data.MosueWheel.Direction = Key_Mouse_WheelUp;

            Event->Data.MosueWheel.Mod = mas_internal_win32_key_mods();
        } 
        return 0;

    case WM_LBUTTONDOWN:   mas_internal_event_add_mouse_button(VK_LBUTTON, KeyState_Press);       return 0;
    case WM_LBUTTONUP:	   mas_internal_event_add_mouse_button(VK_LBUTTON, KeyState_Release);     return 0;
    case WM_LBUTTONDBLCLK: mas_internal_event_add_mouse_button(VK_LBUTTON, KeyState_DoubleClick); return 0;
    case WM_MBUTTONDOWN:   mas_internal_event_add_mouse_button(VK_MBUTTON, KeyState_Press);       return 0;
    case WM_MBUTTONUP:	   mas_internal_event_add_mouse_button(VK_MBUTTON, KeyState_Release);     return 0;
    case WM_MBUTTONDBLCLK: mas_internal_event_add_mouse_button(VK_MBUTTON, KeyState_DoubleClick); return 0;
    case WM_RBUTTONDOWN:   mas_internal_event_add_mouse_button(VK_RBUTTON, KeyState_Press);       return 0;
    case WM_RBUTTONUP:	   mas_internal_event_add_mouse_button(VK_RBUTTON, KeyState_Release);     return 0;
    case WM_RBUTTONDBLCLK: mas_internal_event_add_mouse_button(VK_RBUTTON, KeyState_DoubleClick); return 0;
    case WM_XBUTTONDOWN:   mas_internal_event_add_mouse_button(Wparam,     KeyState_Press);       return 0;
    case WM_XBUTTONUP:     mas_internal_event_add_mouse_button(Wparam,     KeyState_Release);     return 0;
    case WM_XBUTTONDBLCLK: mas_internal_event_add_mouse_button(Wparam,     KeyState_DoubleClick); return 0;

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
			BOOL IsExtendedKey = (KeyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix
			if (IsExtendedKey)
				ScanCode = MAKEWORD(ScanCode, 0xE0);

			WORD ExtKeyCode = 0;
			switch (VKCode)
			{
			case VK_CONTROL: 
			case VK_SHIFT:
			case VK_MENU:
                return 0; // ignore when key is a modifier
				//ExtKeyCode = LOWORD(MapVirtualKey(ScanCode, MAPVK_VSC_TO_VK_EX));
				//switch (ExtKeyCode)
				//{
				//case VK_LCONTROL: VKCode = VK_LCONTROL; break;
				//case VK_LSHIFT:   VKCode = VK_LSHIFT;   break;
				//case VK_LMENU:    VKCode = VK_LMENU;    break;
				//case VK_RCONTROL: VKCode = VK_RCONTROL; break;
				//case VK_RSHIFT:   VKCode = VK_RSHIFT;   break;
				//case VK_RMENU:    VKCode = VK_RMENU;    break;
				//}
				//break;
			}

			if (IsKeyReleased)
				mas_internal_event_add_keyboard_key(VKCode, KeyState_Release);
			else if (WasKeyDown)
				mas_internal_event_add_keyboard_key(VKCode, KeyState_Repeat);
			else
				mas_internal_event_add_keyboard_key(VKCode, KeyState_Press);;
        }
        return 0;
    
    case WM_CHAR:
    case WM_SYSCHAR:
        Event = mas_impl_event_add(EventType_Text_Enter);
        Event->Data.Letter = Wparam;
        return 0;
    }

    return DefWindowProc(Hwnd, Msg, Wparam, Lparam);
}
