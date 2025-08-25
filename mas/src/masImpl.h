#pragma once

#include "masTypes.h"


/***************************************************************************************************************************
* MACROS:
****************************************************************************************************************************/
#define MAS_IMPL_MALLOC(type, size)       (type*)mas_impl_memory_alloc (size,               "MAS_IMPL", __FILE__, __LINE__)
#define MAS_IMPL_REALLOC(type, ptr, size) (type*)mas_impl_memory_resize(ptr,          size, "MAS_IMPL", __FILE__, __LINE__)
#define MAS_IMPL_FREE(ptr)                       mas_impl_memory_free  ((void**)&ptr,       "MAS_IMPL", __FILE__, __LINE__)


/***************************************************************************************************************************
* MEMORY:
****************************************************************************************************************************/
void* mas_impl_memory_alloc(uint64_t Size, const char* Source, const char* File, uint32_t Line);
void* mas_impl_memory_resize(void* Mem, uint64_t Size, const char* Source, const char* File, uint32_t Line);
void  mas_impl_memory_free(void** Mem, const char* Source, const char* File, uint32_t Line);
void  mas_impl_memory_copy(void* Dest, const void* Src, uint64_t Size);
void  mas_impl_memory_move(void* Dest, const void* Src, uint64_t Size);
void  mas_impl_memory_zero(void* Mem, uint64_t Size);
void  mas_impl_memory_set(void* Mem, int32_t Value, uint64_t Size);
void  mas_impl_memory_dump();
void  mas_impl_memory_leak_detect();


/***************************************************************************************************************************
* WINDOW: PARTIALLY DONE, PENDING( MOUSE_ENABLE_CAPTURE, ENABLE_FULLSCREEN )
****************************************************************************************************************************/
bool        mas_impl_window_init(const char* Title, int32_t Width, int32_t Height);
void        mas_impl_window_deinit();
void*       mas_impl_window_handle();
const char* mas_impl_window_title();
void        mas_impl_window_get_pos(int32_t* x, int32_t* y);
void        mas_impl_window_get_size(int32_t* w, int32_t* h);
void        mas_impl_window_get_draw_area_size(int32_t* w, int32_t* h);
void        mas_impl_window_set_pos(int32_t x, int32_t y);
void        mas_impl_window_set_size(int32_t w, int32_t h);
void        mas_impl_window_show(bool EnableVisibility);
bool        mas_impl_window_peek_messages(); 
void        mas_impl_window_enable_fullscreen(bool EnableFullScreen);      // TODO: 
void        mas_impl_window_mouse_show(bool EnableVisibility);             // TODO: 
void        mas_impl_window_mouse_enable_capture(bool EnableMouseCapture); // TODO:  
void        mas_impl_window_mouse_pos_in_screen(int32_t* x, int32_t* y);
void        mas_impl_window_mouse_pos_in_window(int32_t* x, int32_t* y);


/***************************************************************************************************************************
* TIME: DONE
****************************************************************************************************************************/
void   mas_impl_time_init();
void   mas_impl_time_calculate_elapsed();
double mas_impl_time_app();
double mas_impl_time_elapsed();
double mas_impl_time_now();


/***************************************************************************************************************************
* EVENT: PARITALLY DONE( NOT STABLE CAUSING BUGS RAPIDLY WITH CONTINOUS EVENTS -> MOUSE_MOVE, WINDOW_MOVE, WINDOW_RESIZE)
****************************************************************************************************************************/
typedef enum masEventType_
{
    EventType_None,
    EventType_Mouse_Leave,
    EventType_Mouse_Enter,
    EventType_Button,
    EventType_Text_Enter,
    EventType_Device_Changes,

    EventType_Count
    
} masEventType;

typedef struct masEvent_
{
    double       TimeStamp;
    masEventType Type;
    masInputUser InputUser;

    union 
    {
        struct 
        {
            masInputKey      Code;
            masInputKeyMod   Mod;
            masInputKeyState State;
            
        } Key;

        struct { int32_t x; int32_t y; } Pos;
        struct { int32_t w; int32_t h; } Size;
        uint32_t Unicode;

    } Data;
    
} masEvent;

void      mas_impl_event_init();
void      mas_impl_event_deinit();
masEvent* mas_impl_event_add(masEventType EventType);
bool      mas_impl_event_get(masEvent* Event);


/***************************************************************************************************************************
* INPUT: PARTIALLY DONE, PENDING( ON_TEXT_ENTER, CONTROLLERS_CONNECTION_CALLBACK, FEEDBACK_RUMBLE( NOT_WORKING_CORRECTLY ))
****************************************************************************************************************************/
bool mas_impl_input_init();
void mas_impl_input_deinit();
void mas_impl_input_tick();
void mas_impl_input_on_key(masInputUser User, masInputKey Key, masInputKeyState KeyState, masInputKeyMod KeyMod, double TimeStamp);
void mas_impl_input_on_axis(masInputUser User, masInputAxis Axis, float Value);
void mas_impl_input_on_text_enter(masInputUser User, uint32_t Unicode); // TODO: 
void mas_impl_input_clear();

bool  mas_impl_input_key_state(masInputUser User, masInputKeyState KeyState, masInputKey Key, masInputKeyMod KeyMod);
float mas_impl_input_axis_value(masInputUser User, masInputAxis Axis);
void  mas_impl_input_controller_connection_callback(masInputControllerConnectionCallback Callback); // TODO: 

void mas_impl_input_controller_init();
void mas_impl_input_controller_deinit();
void mas_impl_input_controller_check_connection();
void mas_impl_input_controller_set_deadzone(masInputUser User, float LAnalog, float RAnalog);
void mas_impl_input_controller_set_threshold(masInputUser User, float LTrigger, float RTrigger);
void mas_impl_input_controller_restore_setting(masInputUser User);
void mas_impl_input_controller_feedback_rumble(masInputUser User, uint16_t LMotorSpeed, uint16_t RMotorSpeed); // TODO:
void mas_impl_input_controller_tick();


/***************************************************************************************************************************
* DIRECTORY: DONE
****************************************************************************************************************************/
bool           mas_impl_directory_init();
void           mas_impl_directory_deinit();
int32_t        mas_impl_directory_current_path(char* Path, int32_t PathSize);

bool           mas_impl_directory_find_file(const char* TargetPath, const char* FileName, char* OutBuf, int32_t BufSize);
masFileGroup*  mas_impl_directory_find_files(const char* TargetPath, const char* TargetName);
masFileGroup*  mas_impl_directory_find_mix_files(const char* TargetPath, const char** TargetList, int32_t TargetCount);
const masFile* mas_impl_directory_file_group_next_file(masFileGroup* FileGroup);
const char* mas_impl_directory_file_path(const masFile* File);
uint32_t       mas_impl_directory_file_group_file_count(masFileGroup* FileGroup);
void           mas_impl_directory_file_group_destroy(masFileGroup** FileGroup);


/***************************************************************************************************************************
* LOG: IN PROGRESS
****************************************************************************************************************************/
void mas_impl_log(const char* Text, ...);
void mas_impl_log_va_list(const char* Text, va_list Args);