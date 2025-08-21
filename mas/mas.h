#pragma once

#include "src/masTypes.h"


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
*    - Math
*    - Log
*/


/********************************************************************************************************
* INITIALIZATION
*********************************************************************************************************/
bool mas_init(const masChar* Title, int32_t Width, int32_t Height);
void mas_terminate();
bool mas_is_running();


/********************************************************************************************************
* WINDOW
*********************************************************************************************************/
void mas_window_set_pos(int32_t x, int32_t y);
void mas_window_set_size(int32_t w, int32_t h);
void mas_window_get_pos(int32_t* x, int32_t* y);
void mas_window_get_size(int32_t* w, int32_t* h);
void mas_window_get_draw_area_size(int32_t* w, int32_t* h);
void mas_window_enable_fullscreen(bool EnableFullScreen);
void mas_window_show(bool EnableVisibility);
void mas_window_mouse_show(bool EnableVisibility);
void mas_window_mouse_enable_capture(bool EnableCapture);
void mas_window_mouse_set_visibility(bool EnableVisibility);
void mas_window_mouse_pos_in_screen(int32_t* x, int32_t* y);
void mas_window_mouse_pos_in_window(int32_t* x, int32_t* y);


/********************************************************************************************************
* TIME
*********************************************************************************************************/
double mas_time_now();
double mas_time_app();
double mas_time_elapsed();



/********************************************************************************************************
* INPUT
*********************************************************************************************************/
bool  mas_input_key_state(masInputUser User, masInputKey Key, masInputKeyState State, uint8_t KeyMod_);
float mas_input_axis_value(masInputUser User, masInputAxis Key, masInputKeyMod KeyMod);
void  mas_input_controller_connection_callback(masInputControllerConnectionCallback Callback);
void  mas_input_controller_feedback_rumble(masInputUser User, uint16_t LMotorSpeed, uint16_t RMotorSpeed);
void  mas_input_controller_set_deadzone(masInputUser User, float LAnalog, float RAnalog);
void  mas_input_controller_set_threshold(masInputUser User, float LTrigger, float RTrigger);
void  mas_input_controller_restore_setting(masInputUser User);

/********************************************************************************************************
* DIRECTORY
*********************************************************************************************************/
int32_t        mas_directory_current_path(masChar* Path, int32_t PathSize);
bool           mas_directory_find_file(const masChar* TargetPath, const masChar* FileName, masChar* OutBuf, int32_t BufSize);
masFileGroup*  mas_directory_find_files(const masChar* TargetPath, const masChar* TargetName);
masFileGroup*  mas_directory_find_mix_files(const masChar* TargetPath, const masChar** TargetList, int32_t TargetCount);
const masFile* mas_directory_file_group_next_file(masFileGroup* FileGroup);
const masChar* mas_directory_file_path(const masFile* File);
uint32_t       mas_directory_file_group_file_count(masFileGroup* FileGroup);
void           mas_directory_file_group_destroy(masFileGroup** FileGroup);


/********************************************************************************************************
*
*********************************************************************************************************/
void mas_log(const masChar* Text, ...);

//struct masLog;
//masLog* mas_log_create(const char* Name);
//void    mas_log_destroy(masLog** Log);
//void    mas_log_error(masLog* Log, const char* Fmt, ...);
//void    mas_log_info(masLog* Log, const char* Fmt, ...);
//void    mas_log_warning(masLog* Log, const char* Fmt, ...);


#if 0
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


/********************************************************************************************************
*
*********************************************************************************************************/
struct masVec3;
struct masVec4;
struct masMatrix;
#endif