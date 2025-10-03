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
* INITIALIZATION: DONE
*********************************************************************************************************/
bool mas_init(const char* Title, int32_t Width, int32_t Height);
void mas_terminate();
bool mas_is_running();


/***************************************************************************************************************************
* MEMORY: DONE
****************************************************************************************************************************/
void* mas_memory_alloc(uint64_t Size, const char* File, uint32_t Line);
void* mas_memory_resize(void* Mem, uint64_t Size, const char* File, uint32_t Line);
void  mas_memory_free(void** Mem, const char* File, uint32_t Line);
void  mas_memory_copy(void* Dest, const void* Src, uint64_t Size);
void  mas_memory_move(void* Dest, const void* Src, uint64_t Size);
void  mas_memory_zero(void* Mem, uint64_t Size);
void  mas_memory_set(void* Mem, int32_t Value, uint64_t Size);
void  mas_memory_dump();

#define MAS_MALLOC(type, size)       (type*)mas_memory_alloc(size,        __FILE__, __LINE__)
#define MAS_REALLOC(type, ptr, size) (type*)mas_memory_resize(ptr, size,  __FILE__, __LINE__)
#define MAS_FREE(ptr)                       mas_memory_free((void**)&ptr, __FILE__, __LINE__)


/********************************************************************************************************
* WINDOW: DONE
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
* TIME: DONE
*********************************************************************************************************/
double mas_time_now();
double mas_time_app();
double mas_time_elapsed();


/********************************************************************************************************
* INPUT: PARTIALLY DONE
*********************************************************************************************************/
bool  mas_input_key_state(masInputUser User, masInputKey Key, masInputKeyState State, uint8_t KeyMod_);
float mas_input_axis_value(masInputUser User, masInputAxis Key, masInputKeyMod KeyMod);
void  mas_input_controller_connection_callback(masInputControllerConnectionCallback Callback);
void  mas_input_controller_feedback_rumble(masInputUser User, uint16_t LMotorSpeed, uint16_t RMotorSpeed);
void  mas_input_controller_set_deadzone(masInputUser User, float LAnalog, float RAnalog);
void  mas_input_controller_set_threshold(masInputUser User, float LTrigger, float RTrigger);
void  mas_input_controller_restore_setting(masInputUser User);


/********************************************************************************************************
* DIRECTORY: DONE
*********************************************************************************************************/
int32_t        mas_directory_current_path(char* Path, int32_t PathSize);
bool           mas_directory_find_file(const char* TargetPath, const char* FileName, char* OutBuf, int32_t BufSize);
masFileGroup*  mas_directory_find_files(const char* TargetPath, const char* TargetName);
masFileGroup*  mas_directory_find_mix_files(const char* TargetPath, const char** TargetList, int32_t TargetCount);
const masFile* mas_directory_file_group_next_file(masFileGroup* FileGroup);
const char*    mas_directory_file_path(const masFile* File);
uint32_t       mas_directory_file_group_file_count(masFileGroup* FileGroup);
void           mas_directory_file_group_destroy(masFileGroup** FileGroup);


/********************************************************************************************************
* LOG: DONE
*********************************************************************************************************/
void mas_log(const char* Text, ...);

#define MAS_LOG(Text, ...)         mas_log("\033[0;37m[  MAS_GAME  ]\033[0m: "##Text##"\n", ##__VA_ARGS__)
#define MAS_LOG_INFO(Text, ...)    mas_log("\033[0;37m[  MAS_GAME  ]\033[0m: \033[1;32m"##Text##"\033[0m\n", ##__VA_ARGS__)
#define MAS_LOG_WARNING(Text, ...) mas_log("\033[0;37m[  MAS_GAME  ]\033[0m: \033[1;33m"##Text##"\033[0m\n", ##__VA_ARGS__)
#define MAS_LOG_ERROR(Text, ...)   mas_log("\033[0;37m[  MAS_GAME  ]\033[0m: \033[1;31m"##Text##"\033[0m\n", ##__VA_ARGS__)


/***************************************************************************************************************************
* ASSERT: DONE
****************************************************************************************************************************/
void mas_assert(bool Condition, const char* Desc, const char* ErrorMsg, ...);

#define MAS_ASSERT(Condition, Title, Msg, ...) do { mas_assert(Condition, Title, Msg, ##__VA_ARGS__); }while(0)


/***************************************************************************************************************************
* MATH: IN PROGRESS
****************************************************************************************************************************/
float mas_math_float_clamp(float Value, float Min, float Max);
float mas_math_float_min(float V0, float V1);
float mas_math_float_max(float V0, float V1);
float mas_math_deg_to_rad(float Degree);
float mas_math_rad_to_deg(float Radian);

// 2D Vector
void  mas_math_vec2_init(masVec2* Out);
void  mas_math_vec2_set(masVec2* Out, float x, float y);
void  mas_math_vec2_add(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_math_vec2_sub(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_math_vec2_scale(masVec2* Out, const masVec2* V, float Scaler);
void  mas_math_vec2_mul(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_math_vec2_div_float(masVec2* Out, const masVec2* V, float Scaler);
void  mas_math_vec2_div(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_math_vec2_negate(masVec2* Out, const masVec2* V);
void  mas_math_vec2_min(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_math_vec2_max(masVec2* Out, const masVec2* V0, const masVec2* V1);
float mas_math_vec2_distance(const masVec2* V0, const masVec2* V1);
float mas_math_vec2_mag(const masVec2* V);
void  mas_math_vec2_perp(masVec2* Out, const masVec2* V);
void  mas_math_vec2_norm(masVec2* Out, const masVec2* V);
float mas_math_vec2_dot(const masVec2* V0, const masVec2* V1);
void  mas_math_vec2_lerp(masVec2* Out, const masVec2* V0, const masVec2* V1, float T);
void  mas_math_vec2_clamp(masVec2* Out, const masVec2* V, float Min, float Max);
bool  mas_math_vec2_equals(const masVec2* V0, const masVec2* V1);
bool  mas_math_vec2_near(const masVec2* V0, const masVec2* V1, const masVec2* Epsilon);
void  mas_math_vec2_reflect(masVec2* Out, const masVec2* V, const masVec2* Normal);
float mas_math_vec2_angle(const masVec2* V0, const masVec2* V1);


// Vec3 API
void   mas_vec3_set(masVec3 * Out, float x, float y, float z);
void   mas_vec3_add(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_vec3_sub(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_vec3_mul_scale(masVec3 * Out, const masVec3 * V, float Scaler);
void   mas_vec3_mul(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_vec3_div_scale(masVec3 * Out, const masVec3 * V, float Scaler);
void   mas_vec3_div(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_vec3_negate(masVec3 * Out);
float  mas_vec3_length(const masVec3 * V);
float  mas_vec3_length_sq(const masVec3 * V);
void   mas_vec3_norm(masVec3 * Out, const masVec3 * V);
float  mas_vec3_dot(const masVec3 * V0, const masVec3 * V1);
void   mas_vec3_cross(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_vec3_min(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_vec3_max(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_vec3_lerp(masVec3 * Out, const masVec3 * V0, const masVec3 * V1, float T);
float  mas_vec3_angle(const masVec2 * V0, const masVec2 * V1);
void   mas_vec3_reflect(masVec3 * Out, const masVec3 * V, const masVec3 * Normal);
void   mas_vec3_rotate_quaternion(masVec3 * Out, const masVec3 * V, const masQuaternion * Quat);
void   mas_vec3_perpendicular(masVec3 * Out, const masVec3 * V);
void   mas_vec3_clamp(masVec3 * Out, const masVec3 * V, float Min, float Max);
bool   mas_vec3_equals(const masVec3 * V0, const masVec3 * V1);
float  mas_vec3_distance(const masVec3 * V0, const masVec3 * V1);
float  mas_vec3_distance_sq(const masVec3 * V0, const masVec3 * V1);
void   mas_vec3_direction(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);


// Euler
void mas_math_euler_from_quaternion(masVec3 * Out, const masQuaternion * Quaternion);


// Quaternion API
void mas_math_quaternion_init(masQuaternion * Out);
void mas_math_quaternion_from_euler(masQuaternion * Out, const masVec3 * Euler);


// Matrix API
void mas_math_matrix_init(masMatrix * Out);
void mas_math_matrix_multiply(masMatrix * Out, const masMatrix * M0, const masMatrix * M1);
void mas_math_matrix_transpose(masMatrix * Out, const masMatrix * M);
void mas_math_matrix_inverse(masMatrix * Out, const masMatrix * M);
void mas_math_matrix_translate(masMatrix * Out, const masMatrix * M, const masVec3 * V);
void mas_math_matrix_rotate_euler(masMatrix * Out, const masMatrix * M, const masVec3 * Euler);
void mas_math_matrix_rotate_quaternion(masMatrix * Out, const masMatrix * M, const masQuaternion * Q);
void mas_math_matrix_scale(masMatrix * Out, const masMatrix * M, const masVec3 * V);
void mas_math_matrix_transform(masMatrix * Out, const masVec3 * Translate, const masVec3 * Euler, const masVec3 * Scale);
void mas_math_matrix_perspective(masMatrix * Out, float Fov, float AspectRatio, float NearZ, float FarZ);
void mas_math_matrix_orthographic(masMatrix * Out, float ViewWidth, float ViewHeight, float NearZ, float FarZ);
void mas_math_matrix_view();
void mat_math_matrix_decompose(const masMatrix * M, masVec3 * OutScale, masQuaternion * OutQuat, masVec3 * OutTranslate);


/***************************************************************************************************************************
* STRING: IN PROGRESS
****************************************************************************************************************************/
masString*  mas_string_create_from_cstr(const char* Text, ...);
masString*  mas_string_create_from_size(uint32_t Size);
masString*  mas_string_copy(const masString * String);
void        mas_string_destroy(masString * *String);
void        mas_string_append_cstr(masString * *Out, const char* Text, ...);
void        mas_string_insert_cstr(masString * *Out, int32_t At, const char* Text, ...);
void        mas_string_append(masString * *Out, const masString * String);
void        mas_string_insert(masString * *Out, int32_t At, const masString * String);
int32_t     mas_string_find_first(masString * String, const char* Target, uint32_t TargetLen);
int32_t     mas_string_find_last(masString * String, const char* Target, uint32_t TargetLen);
uint64_t    mas_string_hash(const masString * String);
masString*  mas_string_substring(const masString * String, int32_t Index, int32_t Size);
bool        mas_string_compare_cstr(const masString * String, const char* Text);
bool        mas_string_compare(const masString * String1, const masString * String2);
uint64_t    mas_string_length(const masString * String);
const char* mas_string_cstr(const masString * String);


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
#endif
