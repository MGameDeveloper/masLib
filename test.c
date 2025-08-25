
//#define MAS_UNICODE
#include "mas/mas.h"

#define MAS_ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

int32_t main(int32_t argc, const char** argv)
{
    if(!mas_init("masFramework", 800, 600))
    {
        mas_terminate();
        return -1;
    }
    mas_window_show(true);

    char cwd_path[256];
    int32_t cwd_path_len = mas_directory_current_path(cwd_path, 256);
    
    char exe_path[512] = { 0 };
    if(mas_directory_find_file("D:\\Open_Source_Project", "blender.exe", exe_path, 512))
        mas_log("EXE_PATH: %s\n", exe_path);

    //masFileGroup* FileGroup = mas_directory_find_files("D:\\Open_Source_Project", ".png");

    const char *TextureExtList[] = { ".png", ".jpeg", ".jpg", ".tga", ".dds" };
    int32_t        TextureExtCount  = MAS_ARRAY_SIZE(TextureExtList);
    masFileGroup  *TextureFiles     = mas_directory_find_mix_files("D:\\Open_Source_Project", TextureExtList, TextureExtCount);

    const         char* ModelExtList[] = { ".fbx", ".gltf", ".obj", ".blender" };
    int32_t       ModelExtCount           = MAS_ARRAY_SIZE(ModelExtList);
    masFileGroup *ModelFiles              = mas_directory_find_mix_files("D:\\Open_Source_Project", ModelExtList, ModelExtCount);

    const masFile* File = NULL;
    while(File = mas_directory_file_group_next_file(TextureFiles))
        mas_log("TEXTURE_PATH: %s\n",  mas_directory_file_path(File));

    //while(File = mas_directory_file_group_next_file(ModelFiles))
    //    mas_log("MODEL_PATH: %s\n",  mas_directory_file_path(File));

    mas_log("\n:: TEXTURE_COUNT: %u\n", mas_directory_file_group_file_count(TextureFiles));
    //mas_log("\n:: MODEL_COUNT:   %u\n", mas_directory_file_group_file_count(ModelFiles));

    mas_directory_file_group_destroy(&TextureFiles);

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