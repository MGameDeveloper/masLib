#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


void mas_impl_log(const char* Text, ...)
{
    char buf[2048];
    memset(buf, 0, 2048);

    time_t Time = time(NULL);
    struct tm Tm;
    localtime_s(&Tm, &Time);

    int16_t Year   = Tm.tm_year + 1900;
    int16_t Month  = Tm.tm_mon;
    int16_t Day    = Tm.tm_wday;
    int16_t Hour   = Tm.tm_hour;
    int16_t Minute = Tm.tm_min;
    int16_t Second = Tm.tm_sec;
    bool    IsPM = false;
    if (Hour - 12 > 0)
    {
        Hour -= 12;
        IsPM = true;
    }

    va_list Args;
    va_start(Args, Text);
    int32_t Count = sprintf_s(buf, 2048, "\033[90m[%d-%02d-%02d %02d:%02d:%02d %s]\033[0m", Year, Month, Day, Hour, Minute, Second, IsPM ? "PM" : "AM");
    vsprintf_s(buf + Count, 2048 - Count, Text, Args);
    va_end(Args);

    printf(buf);
}

void mas_impl_log_va_list(const char* Text, va_list Args)
{
    char buf[2048];
    memset(buf, 0, 2048);

    time_t Time = time(NULL);
    struct tm Tm;
    localtime_s(&Tm, &Time);

    int16_t Year   = Tm.tm_year + 1900;
    int16_t Month  = Tm.tm_mon;
    int16_t Day    = Tm.tm_wday;
    int16_t Hour   = Tm.tm_hour;
    int16_t Minute = Tm.tm_min;
    int16_t Second = Tm.tm_sec;
    bool    IsPM = false;
    if (Hour - 12 > 0)
    {
        Hour -= 12;
        IsPM = true;
    }

    int32_t Count = sprintf_s(buf, 2048, "\033[90m[%d-%02d-%02d %02d:%02d:%02d %s]\033[0m", Year, Month, Day, Hour, Minute, Second, IsPM ? "PM" : "AM");
    vsprintf_s(buf + Count, 2048 - Count, Text, Args);

    printf(buf);
}