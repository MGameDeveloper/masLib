#include <stdint.h>

/*
* API_CONTENT_TABLE:
*    - Initialization & Cleaning API
*    - Window
*    - Time
*    - Input
*    - Directory Searching & Current Path Query
*    - Directory Watcher for changes
*    - String
*    - Hash Table
*    - Memory
*/


/********************************************************************************************************
*
*********************************************************************************************************/
bool mas_init(const char* Title, int32_t Width, int32_t Height);
bool mas_is_terminated();
void mas_deinit();


/********************************************************************************************************
*
*********************************************************************************************************/
struct masWindowInfo
{
    void       *Handle;
    const char *Title;
    int32_t     PosX;
    int32_t     PosY;
    int32_t     Width;
    int32_t     Height;
    int32_t     ClientWidth;
    int32_t     ClientHeight;
};

const masWindowInfo* mas_window_info();
void                 mas_window_set_pos(int32_t x, int32_t y);
void                 mas_window_set_size(int32_t w, int32_t h);
void                 mas_window_set_fullscreen(bool EnableFullScreen);
void                 mas_window_set_visibility(bool EnableVisibility);


/********************************************************************************************************
*
*********************************************************************************************************/
double mas_time_now();
double mas_time_app();
double mas_time_elapsed_time();


/********************************************************************************************************
*
*********************************************************************************************************/
enum masInputUser
{
    InputUser_1,
    InputUser_2,
    InputUser_3,
    InputUser_4,

    InputUser_Count
};

enum masButtonState
{
    ButtonState_None,
    ButtonState_Press,
    ButtonState_Release,
    ButtonState_Repeat,
    ButtonState_DoubleClick,

    ButtonState_Count
};

enum masKeyMod
{
    KeyMod_None   = 0,
    KeyMod_LCtrl  = (1 << 0),
    KeyMod_RCtrl  = (1 << 1),
    KeyMod_LShift = (1 << 2),
    KeyMod_RShift = (1 << 3),
    KeyMod_LAlt   = (1 << 4),
    KeyMod_RAlt   = (1 << 5),
    KeyMod_LCmd   = (1 << 6),
    KeyMod_RCmd   = (1 << 7),
};
typedef uint8_t KeyMod_;

enum masKey
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
};

bool  mas_input_key_state(masInputUser User, masKey Key, masKeyState State, KeyMod_ KeyMod);
float mas_input_axis_value(masInputUser User, masKey Key, KeyMod_ KeyMod);
void  mas_input_controller_connection_callback(void(*Callback)(masInputUser User, bool IsConnected));
//void  mas_input_feedback();


/********************************************************************************************************
*
*********************************************************************************************************/
struct masFileItr;
struct masFileInfo
{
    const char* Path;
    const char* Name;
    const char* Extension;
};

const char*  mas_directory_current_path();
masFileItr*  mas_directory_search_for(const char* DirectoryPath, const char** Targets, int32_t TargetCount);
masFileItr*  mas_directory_file_itr_next(masFileItr* FileItr);
masFileInfo* mas_directory_file_info_from_itr(masFileItr* FileItr);


/********************************************************************************************************
*
*********************************************************************************************************/
typedef int32_t masDirectoryWatchID;
void                mas_directory_watch_enable(bool value);
masDirectoryWatchID mas_directory_watch_add(const char* DirectoryPath, const char** Targets, int32_t TargetCount, void(*Callback)(void* Data));
void                mas_directory_watch_remove(masDirectoryWatchID DirectoryWatchID);


/********************************************************************************************************
*
*********************************************************************************************************/
struct masString;

masString*  mas_string_create(const char* Text);
masString*  mas_string_create_substring(masString* String, int32_t Offset, int32_t Size);
void        mas_string_destroy(masString** String);
void        mas_string_insert_first(masString** String, const char* Text);
void        mas_string_insert_last(masString** String, const char* Text);
void        mas_string_insert_at(masString** String, int32_t Offset, const char* Text);
int32_t     mas_string_find_first(masString* String, const char* Text);
int32_t     mas_string_find_last(masString* String, const char* Text);
bool        mas_string_compare_to_cstr(masString* String, const char* Text);
bool        mas_string_compare_to_string(masString* String, masString* String);
const char* mas_string_cstr(masString* String);
int32_t     mas_string_length(masString* String);
void        mas_string_replace(masString* String, const char* Target, const char* Value);
void        mas_string_remove(masString* String, const char* Target);


/********************************************************************************************************
*
*********************************************************************************************************/
struct masHashTable;
struct masHashID;

masHashTable* mas_hash_table_create(int32_t ItemCount, int32_t ItemSize, void(*ItemDestructor)(void* UserItem), uint64_t(*HashFunc)(const void* Data, uint64_t Size));
void          mas_hash_table_destroy(masHashTable** HashTable);
masHashID*


/********************************************************************************************************
*
*********************************************************************************************************/
void* mas_memory_alloc(uint64_t size, const char* File, int32_t Line);
void* mas_memory_resize(uint64_t size, const char* File, int32_t Line);
void  mas_memory_free(void** ptr, const char* File, int32_t Line);
void  mas_memory_dump();
void  mas_memory_check_memory_leak();

#define MAS_MALLOC(type, size)       (type*)mas_memory_alloc(size, __FILE__, __LINE__)
#define MAS_REALLOC(type, ptr, size) (type*)mas_memory_resize(ptr, size, __FILE__, __LINE__)
#define MAS_FREE(ptr)                       mas_memory_free((void**)&ptr, __FILE__, __LINE__)