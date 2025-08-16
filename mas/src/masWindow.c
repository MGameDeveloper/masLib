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
} masWindow;

static masWindow Window   = {0};
static HINSTANCE Instance = NULL;


/***************************************************************************************************************************
*
****************************************************************************************************************************/
static LRESULT CALLBACK mas_internal_win32_proc(HWND Hwnd, UINT Msg, WPARAM Wparam, LPARAM Lparam);


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

void mas_impl_window_set_visiblity(bool EnableVisibility)
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

void mas_impl_window_mouse_set_capture(bool EnableMouseCapture)
{
    // TODO: 
}

void mas_impl_window_set_fullscreen(bool EnableFullScreen)
{
    // TODO: 
}

void mas_impl_window_mouse_get_pos(int32_t* x, int32_t* y)
{
    // TODO: 
}


/***************************************************************************************************************************
*
****************************************************************************************************************************/
LRESULT CALLBACK mas_internal_win32_proc(HWND Hwnd, UINT Msg, WPARAM Wparam, LPARAM Lparam)
{
    switch(Msg)
    {
    case WM_CLOSE:
        DestroyWindow(Hwnd);
        PostQuitMessage(0);
        return 0;

    case WM_DEVICECHANGE: 
        mas_impl_input_check_controllers_connection();
        break;

    case WM_SIZE: 
    {
       	RECT Rect = {};
		GetWindowRect(Wnd->Handle, &Rect);
		int32_t Width        = Rect.right - Rect.left;
		int32_t Height       = Rect.bottom - Rect.top;
        int32_t ScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
        int32_t ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		switch (wparam)
		{
		case SIZE_MAXIMIZED: mas_impl_window_set_size(ScreenWidth, ScreenHeight);
		case SIZE_MINIMIZED: mas_impl_window_set_size(0, 0);
		case SIZE_RESTORED:  mas_impl_window_set_size(Width, Height);
		} 

        // Event = mas_impl_event_add(EventType_Window_Resize);
        // Event->WindowSize = { Width, Height };
    }
        return 0;

    case WM_MOVE:
        mas_impl_window_set_pos(GET_X_LPARAM(Lparam), GET_Y_LPARAM(Lparam));
        return 0;

    case WM_MOUSEMOVE:  return 0;
    case WM_MOUSELEAVE: return 0;
    case WM_MOUSEWHEEL: return 0;

    case WM_LBUTTONDOWN:   return 0;
    case WM_LBUTTONUP:	   return 0;
    case WM_LBUTTONDBLCLK: return 0;
    case WM_MBUTTONDOWN:   return 0;
    case WM_MBUTTONUP:	   return 0;
    case WM_MBUTTONDBLCLK: return 0;
    case WM_RBUTTONDOWN:   return 0;
    case WM_RBUTTONUP:	   return 0;
    case WM_RBUTTONDBLCLK: return 0;
    case WM_XBUTTONDOWN:   return 0;
    case WM_XBUTTONUP:     return 0;
    case WM_XBUTTONDBLCLK: return 0;

    case WM_KEYUP:
    case WM_KEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
        return 0;
    
    case WM_CHAR:
    case WM_SYSCHAR:
        return 0;
    }

    return DefWindowProc(Hwnd, Msg, Wparam, Lparam);
}
