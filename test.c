
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
    
    const masChar* search_targets[] = { MAS_TEXT(".h"), MAS_TEXT(".ilk"), MAS_TEXT(".c"), MAS_TEXT(".obj"), MAS_TEXT(".fx") };
    masFileBuf* FileBuf = mas_directory_search_for_files(MAS_TEXT("D:\\Open_Source_Project"), search_targets, 3);
    const masFile* File = NULL;
    while(File = mas_directory_filebuf_next_file(FileBuf))
    {
        mas_log(MAS_TEXT("FILE_INFO:\n"));
        mas_log(MAS_TEXT("    -PATH:      %ls\n"),  File->Path);
        mas_log(MAS_TEXT("    -NAME:      %ls\n"),  File->Name);
        mas_log(MAS_TEXT("    -EXTENSION: %ls\n"),  File->Extension);
        mas_log(MAS_TEXT("    -SIZE:      %llu\n"), File->Size);
        mas_log(MAS_TEXT("\n"));
    }

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