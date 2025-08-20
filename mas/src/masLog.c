#include "masTypes.h"



void mas_impl_log(const masChar* Text, va_list Args)
{
#ifdef MAS_UNICODE
    vwprintf(Text, Args);
#else
    vprintf(Text, Args);
#endif
}