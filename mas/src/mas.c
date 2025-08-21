#include "masImpl.h"


/********************************************************************************************************
* INITIALIZATION
*********************************************************************************************************/
bool mas_init(const masChar* Title, int32_t Width, int32_t Height)
{
    mas_impl_time_init();

    if(!mas_impl_window_init(Title, Width, Height))
        return false;

    mas_impl_directory_init();
    mas_impl_input_init();
    mas_impl_input_controller_init();

    return true;
}

bool mas_is_running()
{
    mas_impl_time_calculate_elapsed();
    mas_impl_input_clear();
    bool IsOnClose = mas_impl_window_peek_messages();

    mas_impl_input_controller_tick();

    masEvent Event = {0};
    while(mas_impl_event_get(&Event))
    {
        switch(Event.Type)
        {
        case EventType_Window_Close: 
            break;

        //case EventType_Window_Maximize: break;
        //case EventType_Window_Minimize: break;

        //case EventType_Window_Resize:
        //    mas_impl_window_set_size(Event.Data.Size.w, Event.Data.Size.h);
        //    break;

        //case EventType_Window_Move:
        //    mas_impl_window_set_pos(Event.Data.Pos.x, Event.Data.Pos.y);
        //    break;

        //case EventType_Mouse_Move : break;
        //case EventType_Mouse_Leave: break;
        //case EventType_Mouse_Enter: break;

        case EventType_Button:
            mas_impl_input_on_key(Event.InputUser, Event.Data.Key.Code, Event.Data.Key.State, Event.Data.Key.Mod, Event.TimeStamp);
            break;

        //case EventType_Text_Enter:
        //    mas_impl_input_on_text_enter(Event.InputUser, Event.Data.Unicode);
        //    break;

        case EventType_Device_Changes:
            mas_impl_input_controller_check_connection();
            break;
        }
    }

    return !IsOnClose;
}

void mas_terminate()
{
    mas_impl_input_controller_deinit();
    mas_impl_input_deinit();
    mas_impl_window_deinit();
}


/********************************************************************************************************
* WINDOW
*********************************************************************************************************/
void mas_window_set_pos                 (int32_t x, int32_t y)   { mas_impl_window_set_pos(x, y);                       }
void mas_window_set_size                (int32_t w, int32_t h)   { mas_impl_window_set_size(w, h);                      }
void mas_window_get_pos                 (int32_t* x, int32_t* y) { mas_impl_window_get_pos(x, y);                       }
void mas_window_get_size                (int32_t* w, int32_t* h) { mas_impl_window_get_size(w, h);                      }
void mas_window_get_draw_area_size      (int32_t* w, int32_t* h) { mas_impl_window_get_draw_area_size(w, h);            }
void mas_window_enable_fullscreen       (bool EnableFullScreen)  { mas_impl_window_enable_fullscreen(EnableFullScreen); }
void mas_window_show                    (bool EnableVisibility)  { mas_impl_window_show(EnableVisibility);              }
void mas_window_mouse_enable_capture    (bool EnableCapture)     { mas_impl_window_mouse_enable_capture(EnableCapture); }
void mas_window_mouse_show              (bool EnableVisibility)  { mas_impl_window_mouse_show(EnableVisibility);        }
void mas_window_mouse_pos_in_screen     (int32_t* x, int32_t* y) { mas_impl_window_mouse_pos_in_screen(x, y);           }
void mas_window_mouse_pos_in_window     (int32_t* x, int32_t* y) { mas_impl_window_mouse_pos_in_window(x, y);           }


/********************************************************************************************************
* TIME
*********************************************************************************************************/
double mas_time_now()     { return mas_impl_time_now(); };
double mas_time_app()     { return mas_impl_time_app(); };
double mas_time_elapsed() { return mas_impl_time_elapsed(); };


/********************************************************************************************************
* INPUT
*********************************************************************************************************/
bool  mas_input_key_state(masInputUser User, masInputKey Key, masInputKeyState State, uint8_t KeyMod_)    { return mas_impl_input_key_state(User, Key, State, KeyMod_);                }
float mas_input_axis_value(masInputUser User, masInputAxis Axis)                                          { return mas_impl_input_axis_value(User, Axis);                              }
void  mas_input_controller_connection_callback(masInputControllerConnectionCallback Callback)             { mas_impl_input_controller_connection_callback(Callback);                   }
void  mas_input_controller_feedback_rumble(masInputUser User, uint16_t LMotorSpeed, uint16_t RMotorSpeed) { mas_impl_input_controller_feedback_rumble(User, LMotorSpeed, RMotorSpeed); }
void  mas_input_controller_set_deadzone(masInputUser User, float LAnalog, float RAnalog)                  { mas_impl_input_controller_set_deadzone(User, LAnalog, RAnalog);            }
void  mas_input_controller_set_threshold(masInputUser User, float LTrigger, float RTrigger)               { mas_impl_input_controller_set_threshold(User, LTrigger, RTrigger);         }
void  mas_input_controller_restore_setting(masInputUser User)                                             { mas_impl_input_controller_restore_setting(User);                           }


/********************************************************************************************************
* DIRECTORY
*********************************************************************************************************/
int32_t        mas_directory_current_path(masChar* Path, int32_t PathSize)                                                   { return mas_impl_directory_current_path(Path, PathSize);                        }
bool           mas_directory_find_file(const masChar* TargetPath, const masChar* FileName, masChar* OutBuf, int32_t BufSize) { return mas_impl_directory_find_file(TargetPath, FileName, OutBuf, BufSize);    }
masFileGroup*  mas_directory_find_files(const masChar* TargetPath, const masChar* TargetName)                                { return mas_impl_directory_find_files(TargetPath, TargetName);                  }
masFileGroup*  mas_directory_find_mix_files(const masChar* TargetPath, const masChar** TargetList, int32_t TargetCount)      { return mas_impl_directory_find_mix_files(TargetPath, TargetList, TargetCount); }
const masFile* mas_directory_file_group_next_file(masFileGroup* FileGroup)                                                   { return mas_impl_directory_file_group_next_file(FileGroup);                     }
const masChar* mas_directory_file_path(const masFile* File)                                                                  { return mas_impl_directory_file_path(File);                                     }
uint32_t       mas_directory_file_group_file_count(masFileGroup* FileGroup)                                                  { return mas_impl_directory_file_group_file_count(FileGroup);                    }
void           mas_directory_file_group_destroy(masFileGroup** FileGroup)                                                    { mas_impl_directory_file_group_destroy(FileGroup);                              }


/********************************************************************************************************
* LOG
*********************************************************************************************************/
void mas_log(const masChar* Text, ...) 
{ 
    va_list Args;
    va_start(Args, Text);
    mas_impl_log(Text, Args);
    va_end(Args);
}