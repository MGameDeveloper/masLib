#include "mas.h"

int32_t main(int32_t argc, const char** argv)
{
    if(!mas_init("masFramework", 800, 600))
    {
        mas_terminate();
        return -1;
    }
    mas_window_show(true);

    MAS_LOG("Normal logging thing");
    MAS_LOG_INFO("INITIALIZE_TIME: %f", mas_time_app());
    MAS_LOG_WARNING("Not working properly");
    MAS_LOG_ERROR("Just another test no.%u", 3221);

    MAS_LOG_INFO("THANK YOU FOR USING THE ENGINE");

    // user_init( params )

    while(mas_is_running())
    {
        // user_tick()
    }

    // user_terminate()

    mas_terminate();
    return 0;
}