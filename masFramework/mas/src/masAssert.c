#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "masImpl.h"


void mas_impl_assert(bool Condition, const char* Desc, const char* ErrorMsg, ...)
{
    if (Condition)
    {
        va_list Args;
        va_start(Args, ErrorMsg);
        mas_impl_assert_va_list(Condition, Desc, ErrorMsg, Args);
        va_end(Args);
    }
}

void mas_impl_assert_va_list(bool Condition, const char* Desc, const char* ErrorMsg, va_list Args)
{
    if (Condition)
    {
        char Buf[512] = { 0 };
        vsprintf(Buf, ErrorMsg, Args);

        MessageBoxA(NULL, Buf, Desc, MB_OK | MB_ICONERROR);
    }
}