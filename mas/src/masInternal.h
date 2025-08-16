#include <stdint.h>
#include <stdbool.h>


/***************************************************************************************************************************
*
****************************************************************************************************************************/



/***************************************************************************************************************************
* WINDOW: PARTIALLY DONE 
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
void        mas_impl_window_set_visiblity(bool EnableVisibility);
bool        mas_impl_window_closed();
void        mas_impl_window_mouse_set_capture(bool EnableMouseCapture); // TODO:   
void        mas_impl_window_set_fullscreen(bool EnableFullScreen);      // TODO:  
void        mas_impl_window_mouse_get_pos(int32_t* x, int32_t* y);      // TODO:


/***************************************************************************************************************************
* TIME: DONE
****************************************************************************************************************************/
void   mas_impl_time_init();
void   mas_impl_time_calculate_elapsed();
double mas_impl_time_app();
double mas_impl_time_elapsed();
double mas_impl_time_now();


/***************************************************************************************************************************
* INPUT: 
****************************************************************************************************************************/