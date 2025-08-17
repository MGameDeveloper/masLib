#include "masInternal.h"

/********************************************************************************************************
* INITIALIZATION
*********************************************************************************************************/
bool mas_init(const char* Title, int32_t Width, int32_t Height)
{
    mas_impl_time_init();

    if(!mas_impl_window_init(Title, Width, Height))
        return false;
}

bool mas_is_running()
{
    mas_impl_time_calculate_elapsed();
    bool IsOnClose = mas_impl_window_peek_messages();

    masEvent Event = {0};
    while(mas_impl_event_get(&Event))
    {
        switch(Event.Type)
        {
        case EventType_Window_Close: 
            break;

        //case EventType_Window_Maximize: break;
        //case EventType_Window_Minimize: break;

        case EventType_Window_Resize:
            mas_impl_window_set_size(Event.Data.Size.x, Event.Data.Size.y);
            break;

        case EventType_Window_Move:
            mas_impl_window_set_pos(Event.Data.Pos.x, Event.Data.Pos.y);
            break;

        //case EventType_Mouse_Move : break;
        //case EventType_Mouse_Leave: break;
        //case EventType_Mouse_Enter: break;

        case EventType_Mouse_Wheel:
            mas_impl_input_on_wheel(Event.InputUser, Event.Data.MouseWheel.Direction, Event.Data.MouseWheel.Mod);
            break;

        case EventType_Button:
            mas_impl_input_on_key(Event.InputUser, Event.Data.Key.Code, Event.Data.Key.State, Event.Data.Key.Mod, Event.TimeStamp);
            break;

        case EventType_Text_Enter:
            mas_impl_input_on_text_enter(Event.InputUser, Event.Data.Unicode);
            break;

        case EventType_Device_Changes:
            mas_impl_input_controller_check_connection();
            break;
        }
    }

    return !IsOnClose;
}

void mas_terminate()
{
    mas_impl_window_deinit();
}


/********************************************************************************************************
* WINDOW
*********************************************************************************************************/
void mas_window_set_pos             (int32_t x, int32_t y)   { mas_impl_window_set_pos(x, y);                         }
void mas_window_set_size            (int32_t w, int32_t h)   { mas_impl_window_set_size(w, h);                        }
void mas_window_get_pos             (int32_t* x, int32_t* y) { mas_impl_window_get_pos(x, y);                         }
void mas_window_get_size            (int32_t* w, int32_t* y) { mas_impl_window_get_size(w, h);                        }
void mas_window_get_draw_area_size  (int32_t* w, int32_t* y) { mas_impl_window_get_draw_area_size(w, h);              }
void mas_window_set_fullscreen      (bool EnableFullScreen)  { mas_impl_window_set_fullscreen(EnableFullScreen);      }
void mas_window_set_visibility      (bool EnableVisibility)  { mas_impl_window_set_visibility(EnableVisibility);      }
void mas_window_mouse_set_capture   (bool EnableCapture)     { mas_impl_window_mouse_set_capture(EnableCapture);      }
void mas_window_mouse_set_visibility(bool EnableVisibility)  { mas_impl_window_mouse_set_visiblity(EnableVisibility); }
void mas_window_mosue_get_pos       (int32_t* x, int32_t* y) { mas_impl_window_mouse_get_pos(x, y);                   }


/********************************************************************************************************
* TIME
*********************************************************************************************************/
double mas_time_now()     { mas_impl_time_now(); };
double mas_time_app()     { mas_impl_time_app(); };
double mas_time_elapsed() { mas_impl_time_elapsed(); };