#include "masTypes.h"



void mas_impl_log(const char* Text, ...)
{
    va_list Args;
    va_start(Args, Text);
    vprintf(Text, Args);
    va_end(Args);
}

void mas_impl_log_va_list(const char* Text, va_list Args)
{
    vprintf(Text, Args);
}