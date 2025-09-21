#include "mas/mas.h"

int32_t main(int32_t argc, const char** argv)
{
    if(!mas_init("masFramework", 800, 600))
    {
        mas_terminate();
        return -1;
    }
    mas_window_show(true);

    mas_log("INITIALIZE_TIME: %f\n", mas_time_app());

    while(mas_is_running())
    {

    }

    mas_terminate();
    return 0;
}