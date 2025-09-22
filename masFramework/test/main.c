#include "mas.h"

int32_t main(int32_t argc, const char** argv)
{
    if(!mas_init("masFramework", 800, 600))
    {
        mas_terminate();
        return -1;
    }
    mas_window_show(true);

    int32_t* list = MAS_MALLOC(int32_t, sizeof(int32_t) * 5);
    MAS_ASSERT(list != NULL, "Entity Allocation", "Allocation failed for list");

    // user_init( params )

    while(mas_is_running())
    {
        // user_tick()
    }

    // user_terminate()

    mas_terminate();
    return 0;
}