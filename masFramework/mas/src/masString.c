#include "masImpl.h"


/***********************************************************************
*
* MAKE SURE TO CONSIDER NULL TERMINATOR CHARACTER AT THE END OF THE STRING
*  IT'S NOT INCLUDED BY DEFAULT ALWAYS ADD ONE IN CASE AND DO THE TESTING TO VALIDATE.
* 
************************************************************************/


struct _masString
{
	char     *Data;
	uint32_t  Size;		
};


/*******************************************************************************************************************
*
********************************************************************************************************************/
static void mas_impl_string_internal_insert(masString** Out, int32_t At, masString* Other)
{
	bool       ResizeSuccess = true;
	masString *OutString    = *Out;
	uint32_t   RequiredSize = sizeof(masString) + (sizeof(char) * (Other->Size + OutString->Size));
	masString *String       = MAS_IMPL_REALLOC(masString, OutString, RequiredSize);
	if (!String)
	{
		ResizeSuccess = false;
		String        = MAS_IMPL_MALLOC(masString, RequiredSize);
		MAS_IMPL_ASSERT(String != NULL, "MAS_STRING", "mas_impl_string_append_cstr: allocation failed for -> %s", Other->Data);
	}

	String->Data = MAS_IMPL_PTR_OFFSET(char, String, sizeof(masString));

	if (At == 0)
	{
		mas_impl_memory_copy(String->Data,               Other->Data,     sizeof(char) * Other->Size);
		mas_impl_memory_copy(String->Data + Other->Size, OutString->Data, sizeof(char) * OutString->Size);
	}
	else if (At == OutString->Size)
	{
		mas_impl_memory_copy(String->Data,                   OutString->Data, sizeof(char) * OutString->Size);
		mas_impl_memory_copy(String->Data + OutString->Size, Other->Data,     sizeof(char) * Other->Size);
	}
	else
	{
		mas_impl_memory_copy(String->Data,                      OutString->Data,      sizeof(char) * At);
		mas_impl_memory_copy(String->Data + At,                 Other->Data,          sizeof(char) * Other->Size);
		mas_impl_memory_copy(String->Data + (Other->Size + At), OutString->Data + At, sizeof(char) * (OutString->Size - At));
	}

	String->Size = OutString->Size + Other->Size;

	if (!ResizeSuccess)
	{
		MAS_IMPL_FREE(*Out);
		*Out = String;
	}
}


/*******************************************************************************************************************
*
********************************************************************************************************************/
masString* mas_impl_string_create_from_cstr(const char* Text, va_list Args)
{
	if (Text)
	{
		char       TempBuf[512] = { 0 };
		int32_t    Count   = vsprintf_s(TempBuf, 512, Text, Args);
		uint64_t   MemSize = sizeof(masString) + (sizeof(char) * Count);
		masString* String  = MAS_IMPL_MALLOC(masString, MemSize);
		MAS_IMPL_ASSERT(String != NULL, "MAS_STRING", "mas_impl_string_create: allocationi failed for -> %s", TempBuf);

		String->Data = MAS_IMPL_PTR_OFFSET(char, String, sizeof(masString));
		String->Size = Count;
		mas_impl_memory_copy(String->Data, TempBuf, sizeof(char) * Count);

		return String;
	}

	return NULL;
}

masString* mas_impl_string_create_from_size(uint32_t Size)
{
	uint64_t   MemSize = sizeof(masString) + (sizeof(char) * Size);
	masString* String = MAS_IMPL_MALLOC(masString, MemSize);
	MAS_IMPL_ASSERT(String != NULL, "MAS_STRING", "mas_impl_string_create: allocationi failed for -> %u", Size);

	String->Data = MAS_IMPL_PTR_OFFSET(char, String, sizeof(masString));
	String->Size = Size;
}

masString* mas_impl_string_copy(const masString* String)
{
	if (String && String->Data && String->Size > 0)
	{
		uint64_t   MemSize    = sizeof(masString) + String->Size;
		masString *StringCopy = MAS_IMPL_MALLOC(masString, MemSize);
		MAS_IMPL_ASSERT(StringCopy != NULL, "MAS_STRING", "mas_impl_string_copy: allocation failed for -> %s", String->Data);

		StringCopy->Data = MAS_IMPL_PTR_OFFSET(char, StringCopy, sizeof(masString));
		StringCopy->Size = String->Size;
		mas_impl_memory_copy(StringCopy->Data, String->Data, sizeof(char) * String->Size);
	}

	return NULL;
}

void mas_impl_string_destroy(masString** String)
{
	if (String && *String)
	{
		MAS_IMPL_FREE(*String);
		*String = NULL;
	}
}

void mas_impl_string_append_cstr(masString** Out, const char* Text, va_list Args)
{
	if (Out && *Out && Text)
	{
		char     TempBuf[512] = { 0 };
		int32_t   Count = vsprintf_s(TempBuf, 512, Text, Args);
		masString Other = { TempBuf, Count };
		mas_impl_string_internal_insert(Out, (*Out)->Size, &Other);
	}
}

void mas_impl_string_insert_cstr(masString** Out, int32_t At, const char* Text, va_list Args)
{
	masString* pOut = NULL;
	if (Out && *Out)
		pOut = *Out;
	else
		return;

	if (At > 0 && At < pOut->Size && Text)
	{
		char      TempBuf[512] = { 0 };
		int32_t   Count = vsprintf_s(TempBuf, 512, Text, Args);
		masString Other = { TempBuf, Count };
		mas_impl_string_internal_insert(Out, At, &Other);
	}
}

void mas_impl_string_append(masString** Out, const masString* String)
{
	if (Out && *Out && String)
	{
		mas_impl_string_internal_insert(Out, (*Out)->Size, String);
	}
}

void mas_impl_string_insert(masString** Out, int32_t At, const masString* String)
{
	if (Out && *Out && String)
	{
		mas_impl_string_internal_insert(Out, At, String);
	}
}

int32_t mas_impl_string_find_first(masString* String, const char* Target, uint32_t TargetLen)
{
	if (String && Target && TargetLen > 0 && TargetLen < String->Size)
	{
		for (uint32_t i = 0; i < String->Size; ++i)
		{
			if ((i + TargetLen) >= String->Size)
				break;

			if (String->Data[i] == Target[0])
			{
				int32_t MatchCount = 0;
				for (uint32_t j = 0; j < TargetLen; ++j)
				{
					if (String->Data[i + j] != Target[j])
						break;
					MatchCount++;
				}

				if (MatchCount == TargetLen)
					return i;
			}
		}
	}

	return -1;
}

int32_t mas_impl_string_find_last(masString* String, const char* Target, uint32_t TargetLen)
{
	if (String && Target && TargetLen > 0 && TargetLen < String->Size)
	{
		for (int32_t i = String->Size - 1; i >= 0; --i)
		{
			if ((i - TargetLen) < 0)
				break;

			if (String->Data[i] == Target[TargetLen - 1])
			{
				int32_t MatchCount = 0;
				for (int32_t j = TargetLen - 1; j >= 0; --j)
				{
					if (String->Data[i - j] != Target[j])
						break;
					MatchCount++;
				}

				if (MatchCount == TargetLen)
					return (i - TargetLen);
			}
		}
	}

	return -1;
}

uint64_t    mas_impl_string_hash(const masString* String);

masString* mas_impl_string_substring(const masString* String, int32_t Index, int32_t Size)
{
	if (String && Index > 0 && (Index + Size) < String->Size && (Index + Size) > 0)
	{
		masString* SubString = mas_impl_string_create_from_size(Size);
		if (!SubString)
			return NULL;
		mas_impl_memory_copy(SubString->Data, String->Data + Index, Size);
		SubString->Size = Size;
		return SubString;
	}

	return NULL;
}

bool mas_impl_string_compare_cstr(const masString* String, const char* Text)
{
	if (String)
	{
		if (strcmp(String->Data, Text) == 0)
			return true;
	}

	return false;
}

bool mas_impl_string_compare(const masString* String1, const masString* String2)
{
	if (String1 && String2)
	{
		if (strcmp(String1->Data, String2->Data) == 0)
			return true;
	}

	return false;
}

uint64_t    mas_impl_string_length(const masString* String)
{
	if (String)
		return String->Size;
	return 0;
}

const char* mas_impl_string_cstr(const masString* String)
{
	if (String)
		return String->Data;
	return NULL;
}