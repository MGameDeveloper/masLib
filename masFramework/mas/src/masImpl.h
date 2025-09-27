#pragma once

#include "masTypes.h"


/***************************************************************************************************************************
* MEMORY:
****************************************************************************************************************************/
void* mas_impl_memory_alloc(uint64_t Size, const char* File, uint32_t Line);
void* mas_impl_memory_resize(void* Mem, uint64_t Size, const char* File, uint32_t Line);
void  mas_impl_memory_free(void* Mem, const char* File, uint32_t Line);
void  mas_impl_memory_copy(void* Dest, const void* Src, uint64_t Size);
void  mas_impl_memory_move(void* Dest, const void* Src, uint64_t Size);
void  mas_impl_memory_zero(void* Mem, uint64_t Size);
void  mas_impl_memory_set(void* Mem, int32_t Value, uint64_t Size);
void  mas_impl_memory_dump();
void  mas_impl_memory_leak_detect();

#define MAS_IMPL_MALLOC(type, size)       (type*)mas_impl_memory_alloc (size,       __FILE__, __LINE__)
#define MAS_IMPL_REALLOC(type, ptr, size) (type*)mas_impl_memory_resize(ptr,  size, __FILE__, __LINE__)
#define MAS_IMPL_FREE(ptr)                       mas_impl_memory_free  (ptr,        __FILE__, __LINE__)


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
* DIRECTORY: DONE [ NEED REFACTORY ]
****************************************************************************************************************************/
bool           mas_impl_directory_init();
void           mas_impl_directory_deinit();
int32_t        mas_impl_directory_current_path(char* Path, int32_t PathSize);

bool           mas_impl_directory_find_file(const char* TargetPath, const char* FileName, char* OutBuf, int32_t BufSize);
masFileGroup*  mas_impl_directory_find_files(const char* TargetPath, const char* TargetName);
masFileGroup*  mas_impl_directory_find_mix_files(const char* TargetPath, const char** TargetList, int32_t TargetCount);
const masFile* mas_impl_directory_file_group_next_file(masFileGroup* FileGroup);
const char*    mas_impl_directory_file_path(const masFile* File);
uint32_t       mas_impl_directory_file_group_file_count(masFileGroup* FileGroup);
void           mas_impl_directory_file_group_destroy(masFileGroup** FileGroup);


/***************************************************************************************************************************
* LOG: IN PROGRESS
****************************************************************************************************************************/
void mas_impl_log(const char* Text, ...);
void mas_impl_log_va_list(const char* Text, va_list Args);

#define MAS_IMPL_LOG(Text, ...)         mas_impl_log("\033[90m[ MAS_ENGINE ]\033[0m: "##Text##"\n\033[0m", ##__VA_ARGS__)
#define MAS_IMPL_LOG_INFO(Text, ...)    mas_impl_log("\033[90m[ MAS_ENGINE ]\033[0m: \033[1;32m"##Text##"\n\033[0m", ##__VA_ARGS__)
#define MAS_IMPL_LOG_WARNING(Text, ...) mas_impl_log("\033[90m[ MAS_ENGINE ]\033[0m: \033[1;33m"##Text##"\n\033[0m", ##__VA_ARGS__)
#define MAS_IMPL_LOG_ERROR(Text, ...)   mas_impl_log("\033[90m[ MAS_ENGINE ]\033[0m: \033[1;31m"##Text##"\n\033[0m", ##__VA_ARGS__)


/***************************************************************************************************************************
* ASSERT: DONE
****************************************************************************************************************************/
void mas_impl_assert(bool Condition, const char* Title, const char* ErrorMsg, ...);
void mas_impl_assert_va_list(bool Condition, const char* Desc, const char* ErrorMsg, va_list Args);
#define MAS_IMPL_ASSERT(Condition, Module, Msg, ...) do { mas_impl_assert(Condition, Module, Msg, ##__VA_ARGS__); } while(0)




/***************************************************************************************************************************
* MATH: IN PROGRESS
****************************************************************************************************************************/
float mas_impl_math_float_clamp(float Value, float Min, float Max);
float mas_impl_math_float_min  (float V0, float V1);
float mas_impl_math_float_max  (float V0, float V1);
float mas_impl_math_deg_to_rad (float Degree);
float mas_impl_math_rad_to_deg (float Radian);


// 2D Vector
void  mas_impl_math_vec2_set(masVec2* Out, float x, float y);
void  mas_impl_math_vec2_add(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_impl_math_vec2_sub(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_impl_math_vec2_mul_scale(masVec2* Out, const masVec2* V, float Scaler);
void  mas_impl_math_vec2_mul(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_impl_math_vec2_div_scale(masVec2* Out, const masVec2* V, float Scaler);
void  mas_impl_math_vec2_div(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_impl_math_vec2_negate(masVec2* Out, const masVec2* V);
void  mas_impl_math_vec2_min(masVec2* Out, const masVec2* V0, const masVec2* V1);
void  mas_impl_math_vec2_max(masVec2* Out, const masVec2* V0, const masVec2* V1);
float mas_impl_math_vec2_distance(const masVec2* V0, const masVec2* V1);
float mas_impl_math_vec2_mag(const masVec2* V);
void  mas_impl_math_vec2_perpendicular(masVec2* Out, const masVec2* V);
void  mas_impl_math_vec2_norm(masVec2* Out, const masVec2* V);
float mas_impl_math_vec2_dot(const masVec2* V0, const masVec2* V1);
void  mas_impl_math_vec2_lerp(masVec2* Out, const masVec2* V0, const masVec2* V1, float T);
void  mas_impl_math_vec2_clamp(masVec2* Out, const masVec2* V, float Min, float Max);
bool  mas_impl_math_vec2_equals(const masVec2* V0, const masVec2* V1);
bool  mas_impl_math_vec2_near(const masVec2* V0, const masVec2* V1, const masVec2* Epsilon);
void  mas_impl_math_vec2_reflect(masVec2* Out, const masVec2* V, const masVec2* Normal);
float mas_impl_math_vec2_angle(const masVec2* V0, const masVec2* V1);


// Vec3 API
void   mas_impl_vec3_set(masVec3 * Out, float x, float y, float z);
void   mas_impl_vec3_add(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_impl_vec3_sub(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_impl_vec3_mul_scale(masVec3 * Out, const masVec3 * V, float Scaler);
void   mas_impl_vec3_mul(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_impl_vec3_div_scale(masVec3 * Out, const masVec3 * V, float Scaler);
void   mas_impl_vec3_div(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_impl_vec3_negate(masVec3 * Out);
float  mas_impl_vec3_length(const masVec3 * V);
float  mas_impl_vec3_length_sq(const masVec3 * V);
void   mas_impl_vec3_norm(masVec3 * Out, const masVec3 * V);
float  mas_impl_vec3_dot(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_impl_vec3_cross(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_impl_vec3_min(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_impl_vec3_max(masVec3 * Out, const masVec3 * V0, const masVec3 * V1);
void   mas_impl_vec3_lerp(masVec3 * Out, const masVec3 * V0, const masVec3 * V1, float T);
float  mas_impl_vec3_angle(const masVec2 * V0, const masVec2 * V1);
void   mas_impl_vec3_reflect(masVec3 * Out, const masVec3 * V, const masVec3 * Normal);
void   mas_impl_vec3_rotate_quaternion(masVec3 * Out, const masVec3 * V, const masVec4 * Quat);
void   mas_impl_vec3_perpendicular(masVec3 * Out, const masVec3 * V);
void   mas_impl_vec3_clamp(masVec3 * Out, const masVec3 * V, float Min, float Max);
bool   mas_impl_vec3_equals(const masVec3 * V0, const masVec3 * V1);
float  mas_impl_vec3_distance(const masVec3* V0, const masVec3* V1);
float  mas_impl_vec3_distance_sq(const masVec3* V0, const masVec3* V1);
void   mas_impl_vec3_direction(masVec3* Out, const masVec3* V0, const masVec3* V1);


// Euler
void mas_impl_math_euler_from_quaternion(masVec3 * Out, const masVec4 * Quaternion);


// Quaternion API
void mas_impl_math_quaternion_init(masVec4 * Out);
void mas_impl_math_quaternion_from_euler(masVec4 * Out, const masVec3 * Euler);


// Matrix API
void mas_impl_math_matrix_init(masMatrix * Out);
void mas_impl_math_matrix_multiply(masMatrix* Out, const masMatrix* M0, const masMatrix* M1);
void mas_impl_math_matrix_transpose(masMatrix* Out, const masMatrix* M);
void mas_impl_math_matrix_inverse(masMatrix* Out, const masMatrix* M);
void mas_impl_math_matrix_translate(masMatrix* Out, const masMatrix* M, const masVec3* V);
void mas_impl_math_matrix_rotate_euler(masMatrix* Out, const masMatrix* M, const masVec3* Euler);
void mas_impl_math_matrix_rotate_quaternion(masMatrix* Out, const masMatrix* M, const masVec4* Q);
void mas_impl_math_matrix_scale(masMatrix* Out, const masMatrix* M, const masVec3* V);
void mas_impl_math_matrix_transform(masMatrix * Out, const masVec3 * Translate, const masVec3 * Euler, const masVec3 * Scale);
void mas_impl_math_matrix_perspective();
void mas_impl_math_matrix_orthographic();
void mas_impl_math_matrix_view();
void mat_impl_math_matrix_decompose();
