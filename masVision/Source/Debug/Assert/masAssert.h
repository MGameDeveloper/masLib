#pragma once

struct masAssert
{
	static void Assert(bool Condition, const char* File, int32_t Line, const char* Description, ...);
};

#define MAS_ASSERT(Condition, Description, ...)\
    masAssert::Assert(Condition, __FILE__, __LINE__, Description, ##__VA_ARGS__)