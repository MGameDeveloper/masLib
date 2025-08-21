
//#define MAS_UNICODE
#include "mas/mas.h"


int32_t main(int32_t argc, const char** argv)
{
    if(!mas_init(MAS_TEXT("masFramework"), 800, 600))
    {
        mas_terminate();
        return -1;
    }
    mas_window_show(true);

    masChar cwd_path[256];
    int32_t cwd_path_len = mas_directory_current_path(cwd_path, 256);
    
    double StartTime = mas_time_now();
    masChar exe_path[512] = { 0 };
    if(mas_directory_find_file(MAS_TEXT("D:\\Open_Source_Project"), MAS_TEXT("blender.exe"), exe_path, 512))
        mas_log(MAS_TEXT("EXE_PATH: %s\n"), exe_path);

    const masFileGroup* FileGroup = mas_directory_find_file_group(MAS_TEXT("D:\\Open_Source_Project"), MAS_TEXT(".png"));
    double Duration = mas_time_now() - StartTime;

    const masFile* File = NULL;
    while(File = mas_directory_file_group_next_file(FileGroup))
        mas_log(MAS_TEXT("FILE_PATH: %s\n"),  mas_directory_file_path(File));
    mas_log(MAS_TEXT("\n:: FILE_COUNT: %u\n"), mas_directory_file_group_file_count(FileGroup));

    while(mas_is_running())
    {
        if (mas_input_key_state(InputUser_1, InputKeyState_Repeat, InputKey_Controller_Square, InputKeyMod_None))
        {
            mas_input_controller_feedback_rumble(InputUser_1, 65535, 65535);
        }

        float LStickX = mas_input_axis_value(InputUser_1, InputAxis_Controller_LStickX, InputKeyMod_None);
        if(LStickX != 0.0f)
            printf("L_STICK_X: %.5f\n", LStickX);

        float R2 = mas_input_axis_value(InputUser_1, InputAxis_Controller_R2, InputKeyMod_None);
        if(R2 != 0.0f)
            printf("R2: %.5f\n", R2);
    }

    mas_terminate();
    return 0;
}