#include <stdint.h>
#include <stdbool.h>

#include "masInputTypes.h"


/***************************************************************************************************************************
* MACROS:
****************************************************************************************************************************/



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
void        mas_impl_window_mouse_enable_capture(bool EnableMouseCapture); // TODO:   
void        mas_impl_window_enable_fullscreen(bool EnableFullScreen);      // TODO:  
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
* EVENT: DONE, MAY BE VISITED OFTEN DUE TO OTHER SYSTEM DECLARING NEW EVENT TYPES
****************************************************************************************************************************/
typedef enum masEventType_
{
    EventType_None,

    EventType_Window_Close,
    EventType_Window_Maximize,
    EventType_Window_Minimize,
    EventType_Window_Resize,
    EventType_Window_Move,
    EventType_Mouse_Move,
    EventType_Mouse_Leave,
    EventType_Mouse_Enter,
    EventType_Mouse_Wheel,
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
            masKey      Code;
            masKeyMod   Mod;
            masKeyState State;
            
        } Key;
        
        struct 
        {
            masKey    Direction;
            masKeyMod Mod;

        } MouseWheel;

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
* INPUT: IN PROGRESS
****************************************************************************************************************************/
bool mas_impl_input_init();
void mas_impl_input_deinit();
void mas_impl_input_on_key(masInputUser User, masKey Key, masKeyState KeyState, masKeyMod KeyMod, double TimeStamp);
void mas_impl_input_on_wheel(masInputUser User, masKey Key, masKeyMod KeyMod);
void mas_impl_input_on_axis(masInputUser User, masKey Key, float Value);
void mas_impl_input_on_text_enter(masInputUser User, uint32_t Unicode);

void mas_impl_input_controller_init();
void mas_impl_input_controller_deinit();
void mas_impl_input_controller_check_connection();
void mas_impl_input_controller_tick();