#include "mas.h"

void test_math()
{

}

void test_string()
{

}

int32_t main(int32_t argc, const char** argv)
{
    if(!mas_init("masFramework", 800, 600))
    {
        mas_terminate();
        return -1;
    }
    mas_window_show(true);

    test_math();
    test_string();

    // user_init( params )

    while(mas_is_running())
    {
        // user_tick()
    }

    // user_terminate()

    mas_terminate();
    return 0;
}