#include "masTypes.h"


/***************************************************************************************************************************
* MACROS:
****************************************************************************************************************************/



/***************************************************************************************************************************
* WINDOW: PARTIALLY DONE, PENDING( MOUSE_ENABLE_CAPTURE, ENABLE_FULLSCREEN )
****************************************************************************************************************************/
bool           mas_impl_window_init(const masChar* Title, int32_t Width, int32_t Height);
void           mas_impl_window_deinit();
void*          mas_impl_window_handle();
const masChar* mas_impl_window_title();
void           mas_impl_window_get_pos(int32_t* x, int32_t* y);
void           mas_impl_window_get_size(int32_t* w, int32_t* h);
void           mas_impl_window_get_draw_area_size(int32_t* w, int32_t* h);
void           mas_impl_window_set_pos(int32_t x, int32_t y);
void           mas_impl_window_set_size(int32_t w, int32_t h);
void           mas_impl_window_show(bool EnableVisibility);
bool           mas_impl_window_peek_messages(); 
void           mas_impl_window_enable_fullscreen(bool EnableFullScreen);      // TODO: 
void           mas_impl_window_mouse_show(bool EnableVisibility);             // TODO: 
void           mas_impl_window_mouse_enable_capture(bool EnableMouseCapture); // TODO:  
void           mas_impl_window_mouse_pos_in_screen(int32_t* x, int32_t* y);
void           mas_impl_window_mouse_pos_in_window(int32_t* x, int32_t* y);


/***************************************************************************************************************************
* TIME: DONE
****************************************************************************************************************************/
void   mas_impl_time_init();
void   mas_impl_time_calculate_elapsed();
double mas_impl_time_app();
double mas_impl_time_elapsed();
double mas_impl_time_now();


/***************************************************************************************************************************
* EVENT: DONE, MAY BE VISITED OFTEN DUE TO OTHER SYSTEMS DECLARING NEW EVENT TYPES
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
* INPUT: PARTIALLY DONE, PENDING( ON_TEXT_ENTER, CONTROLLERS_CONNECTION_CALLBACK )
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
void mas_impl_input_controller_feedback_rumble(masInputUser User, uint16_t LMotorSpeed, uint16_t RMotorSpeed);
void mas_impl_input_controller_tick();


/***************************************************************************************************************************
* DIRECTORY: DONE
****************************************************************************************************************************/
bool        mas_impl_directory_init();
void        mas_impl_directory_deinit();
int32_t     mas_impl_directory_current_path(masChar* Path, int32_t PathSize);
masFileBuf*    mas_impl_directory_search_for_files(const masChar* DirectoryPath, const masChar** TargetFiles, int32_t TargetCount);
const masFile* mas_impl_directory_filebuf_next_file(masFileBuf* FileBuf);
int32_t        mas_impl_directory_find_folder(const masChar* DirectoryPath, const masChar* FolderName, masChar* FolderPath, int32_t FolderPathSize);


/***************************************************************************************************************************
* LOG: IN PROGRESS
****************************************************************************************************************************/
void mas_impl_log(const masChar* Text, ...);