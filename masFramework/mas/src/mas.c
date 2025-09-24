#include "masImpl.h"


/********************************************************************************************************
* INITIALIZATION
*********************************************************************************************************/
bool mas_init(const char* Title, int32_t Width, int32_t Height)
{
    mas_impl_time_init();

    if(!mas_impl_window_init(Title, Width, Height))
        return false;

    mas_impl_directory_init();
    mas_impl_input_init();
    mas_impl_input_controller_init();
    MAS_IMPL_LOG_INFO("Initialized");
    MAS_IMPL_LOG("Just testing message");
    MAS_IMPL_LOG_WARNING("May cause a problem");
    MAS_IMPL_LOG_ERROR("Allocation game memory failed");
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
    mas_impl_directory_deinit();
    mas_impl_memory_leak_detect();
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
int32_t        mas_directory_current_path(char* Path, int32_t PathSize)                                             { return mas_impl_directory_current_path(Path, PathSize);                        }
bool           mas_directory_find_file(const char* TargetPath, const char* FileName, char* OutBuf, int32_t BufSize) { return mas_impl_directory_find_file(TargetPath, FileName, OutBuf, BufSize);    }
masFileGroup*  mas_directory_find_files(const char* TargetPath, const char* TargetName)                             { return mas_impl_directory_find_files(TargetPath, TargetName);                  }
masFileGroup*  mas_directory_find_mix_files(const char* TargetPath, const char** TargetList, int32_t TargetCount)   { return mas_impl_directory_find_mix_files(TargetPath, TargetList, TargetCount); }
const masFile* mas_directory_file_group_next_file(masFileGroup* FileGroup)                                          { return mas_impl_directory_file_group_next_file(FileGroup);                     }
const char*    mas_directory_file_path(const masFile* File)                                                         { return mas_impl_directory_file_path(File);                                     }
uint32_t       mas_directory_file_group_file_count(masFileGroup* FileGroup)                                         { return mas_impl_directory_file_group_file_count(FileGroup);                    }
void           mas_directory_file_group_destroy(masFileGroup** FileGroup)                                           { mas_impl_directory_file_group_destroy(FileGroup);                              }


/********************************************************************************************************
* LOG:
*********************************************************************************************************/
void mas_log(const char* Text, ...) 
{ 
    va_list Args;
    va_start(Args, Text);
    mas_impl_log_va_list(Text, Args);
    va_end(Args);
}


/********************************************************************************************************
* LOG:
*********************************************************************************************************/
void mas_assert(bool Condition, const char* Title, const char* ErrorMsg, ...)
{
    va_list Args;
    va_start(Args, ErrorMsg);
    mas_impl_assert_va_list(Condition, Title, ErrorMsg, Args);
    va_end(Args);
}


/***************************************************************************************************************************
* MEMORY: 
****************************************************************************************************************************/
void* mas_memory_alloc(uint64_t Size, const char* File, uint32_t Line)             { return mas_impl_memory_alloc(Size, File, Line);       }
void* mas_memory_resize(void* Mem, uint64_t Size, const char* File, uint32_t Line) { return mas_impl_memory_resize(Mem, Size, File, Line); }
void  mas_memory_free(void** Mem, const char* File, uint32_t Line)                 { mas_impl_memory_free(Mem, File, Line);                }
void  mas_memory_copy(void* Dest, const void* Src, uint64_t Size)                  { mas_impl_memory_copy(Dest, Src, Size); }
void  mas_memory_move(void* Dest, const void* Src, uint64_t Size)                  { mas_impl_memory_move(Dest, Src, Size); }
void  mas_memory_zero(void* Mem, uint64_t Size)                                    { mas_impl_memory_zero(Mem, Size);       }
void  mas_memory_set(void* Mem, int32_t Value, uint64_t Size)                      { mas_impl_memory_set(Mem, Value, Size); }
void  mas_memory_dump()                                                            { mas_impl_memory_dump();                }


/***************************************************************************************************************************
* MATH: IN PROGRESS
****************************************************************************************************************************/
float mas_math_float_clamp(float Value, float Min, float Max) { return mas_impl_math_float_clamp(Value, Min, Max); }
float mas_math_float_min(float V0, float V1)                  { return mas_impl_math_float_min  (V0, V1);          }
float mas_math_float_max(float V0, float V1)                  { return mas_impl_math_float_max  (V0, V1);          }
float mas_math_deg_to_rad(float Degree)                       { return mas_impl_math_deg_to_rad (Degree);          }
float mas_math_rad_to_deg(float Radian)                       { return mas_impl_math_rad_to_deg (Radian);          }


// 2D Vector
void  mas_math_vec2_init(masVec2* Out)                                                 { mas_impl_memory_zero(Out, sizeof(masVec2));         }
void  mas_math_vec2_set(masVec2* Out, float x, float y)                                { mas_impl_math_vec2_set(Out, x, y);                  }
void  mas_math_vec2_add(masVec2* Out, const masVec2* V0, const masVec2* V1)            { mas_impl_math_vec2_add(Out, V0, V1);                }
void  mas_math_vec2_sub(masVec2* Out, const masVec2* V0, const masVec2* V1)            { mas_impl_math_vec2_sub(Out, V0, V1);                }
void  mas_math_vec2_scale(masVec2* Out, const masVec2* V, float Scaler)                { mas_impl_math_vec2_scale(Out, V, Scaler);           }
void  mas_math_vec2_mul(masVec2* Out, const masVec2* V0, const masVec2* V1)            { mas_impl_math_vec2_mul(Out, V0, V1);                }
void  mas_math_vec2_div_float(masVec2* Out, const masVec2* V, float Scaler)            { mas_impl_math_vec2_div_float(Out, V, Scaler);       }
void  mas_math_vec2_div(masVec2* Out, const masVec2* V0, const masVec2* V1)            { mas_impl_math_vec2_div(Out, V0, V1);                }
void  mas_math_vec2_negate(masVec2* Out, const masVec2* V)                             { mas_impl_math_vec2_negate(Out, V);                  }
void  mas_math_vec2_min(masVec2* Out, const masVec2* V0, const masVec2* V1)            { mas_impl_math_vec2_min(Out, V0, V1);                }
void  mas_math_vec2_max(masVec2* Out, const masVec2* V0, const masVec2* V1)            { mas_impl_math_vec2_max(Out, V0, V1);                }
float mas_math_vec2_distance(const masVec2* V0, const masVec2* V1)                     { return mas_impl_math_vec2_distance(V0, V1);         }
float mas_math_vec2_mag(const masVec2* V)                                              { return mas_impl_math_vec2_mag(V);                   }
void  mas_math_vec2_perp(masVec2* Out, const masVec2* V)                               { mas_impl_math_vec2_perp(Out, V);                    }
void  mas_math_vec2_norm(masVec2* Out, const masVec2* V)                               { mas_impl_math_vec2_norm(Out, V);                    }
float mas_math_vec2_dot(const masVec2* V0, const masVec2* V1)                          { return mas_impl_math_vec2_dot(V0, V1);              }
void  mas_math_vec2_lerp(masVec2* Out, const masVec2* V0, const masVec2* V1, float T)  { mas_impl_math_vec2_lerp(Out, V0, V1, T);            }
void  mas_math_vec2_clamp(masVec2* Out, const masVec2* V, float Min, float Max)        { mas_impl_math_vec2_clamp(Out, V, Min, Max);         }
bool  mas_math_vec2_equals(const masVec2* V0, const masVec2* V1)                       { return mas_impl_math_vec2_equals(V0, V1);           }
bool  mas_math_vec2_near(const masVec2* V0, const masVec2* V1, const masVec2* Epsilon) { return mas_impl_math_vec2_near(V0, V1, Epsilon);    }
void  mas_math_vec2_reflect(masVec2* Out, const masVec2* V, const masVec2* Normal)     { mas_impl_math_vec2_reflect(Out, V, Normal);         }
float mas_math_vec2_angle(const masVec2* V0, const masVec2* V1)                        { return mas_impl_math_vec2_angle(V0, V1);            }


// 3D Vector