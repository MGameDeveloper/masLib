#include <time.h>
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
    char buf[512];
    memset(buf, 0, 512);

    time_t Time = time(NULL);
    struct tm Tm;
    localtime_s(&Tm, &Time);

    int16_t Year   = Tm.tm_year + 1900;
    int16_t Month  = Tm.tm_mon;
    int16_t Day    = Tm.tm_wday;
    int16_t Hour   = Tm.tm_hour;
    int16_t Minute = Tm.tm_min;
    int16_t Second = Tm.tm_sec;
    if(Hour - 12 > 0)
        Hour -= 12;

    int32_t Count = sprintf_s(buf, 512, "[%d-%02d-%02d %02d:%02d:%02d][ %s ]: ", Year, Month, Day, Hour, Minute, Second, "INFO");
    vsprintf_s(buf + Count, 512 - Count, Text, Args);
    printf(buf);
}