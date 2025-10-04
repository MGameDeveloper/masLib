#include "masAssert.h"

#include <stdint.h>
#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>

void masAssert::Assert(bool Condition, const char* File, int32_t Line, const char* Description, ...)
{
	if (!Condition)
	{
		const uint64_t TempBufSize = 512;
		char TempBuf[TempBufSize] = { 0 };

		va_list Args;
		va_start(Args, Description);
		int32_t Count = sprintf_s(TempBuf,          TempBufSize,        "FILE: %s\nLINE: %d\n", File, Line);
		Count        += vsprintf_s(TempBuf + Count, TempBufSize - Count, Description,           Args);
		va_end(Args);

		MessageBoxA(NULL, TempBuf, "MAS_ASSERT", MB_ICONERROR | MB_OK);
	}
}
