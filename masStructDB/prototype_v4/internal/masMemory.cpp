#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "masMemory.h"


void* masMemory_Malloc(size_t InSize, const char* InFile, const char* InFunc, int InLine)
{
	void* Data = malloc(InSize);
	printf("[ MAS_MEMORY ]: MALLOC[ %llu BYTE ]\n\t[ %s ] ( %d ) %s\n", InSize, InFunc, InLine, InFile);

	return Data;
}

bool masMemory_Realloc(void** InPtr, size_t InSize, const char* InFile, const char* InFunc, int InLine)
{
	void* Data = realloc(InPtr, InSize);
	if (!Data)
	{
		printf("[ MAS_MEMORY ]: REALLOC_FAILED[ %llu BYTE ]\n\t[ %s ] ( %d ) %s\n", InSize, InFunc, InLine, InFile);
		return false;
	}

	*InPtr = Data;

	printf("[ MAS_MEMORY ]: REALLOC[ %llu BYTE ]\n\t[ %s ] ( %d ) %s\n", InSize, InFunc, InLine, InFile);
	return true;
}

void masMemory_Free(void* InPtr, const char* InFile, const char* InFunc, int InLine)
{
	if (InPtr)
	{
		free(InPtr);
		printf("[ MAS_MEMORY ]: FREE\n\t[ %s ] ( %d ) %s\n", InFunc, InLine, InFile);
	}
}
