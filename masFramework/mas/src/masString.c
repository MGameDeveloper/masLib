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

masString* mas_impl_string_create(const char* Text, ...)
{
	if (Text)
	{
		char TempBuf[512] = { 0 };

		va_list Args;
		va_start(Args, Text);
		int32_t Count = vsprintf_s(TempBuf, 512, Text, Args);
		va_end(Args);

		uint64_t   MemSize = sizeof(masString) + (sizeof(char) * Count);
		masString* String = MAS_IMPL_MALLOC(masString, MemSize);
		MAS_IMPL_ASSERT(String != NULL, "MAS_STRING", "mas_impl_string_create: allocationi failed for -> %s", TempBuf);

		mas_impl_memory_copy(String->Data, TempBuf, sizeof(char) * Count);
		String->Size = Count;

		return String;
	}

	return NULL;
}

masString* mas_impl_string_copy(const masString* String)
{
	if (String && String->Data && String->Size > 0)
	{
		uint64_t   MemSize    = sizeof(masString) + String->Size;
		masString *StringCopy = MAS_IMPL_MALLOC(masString, MemSize);
		MAS_IMPL_ASSERT(StringCopy != NULL, "MAS_STRING", "mas_impl_string_copy: allocation failed for -> %s", String->Data);

		mas_impl_memory_copy(StringCopy->Data, String->Data, sizeof(char) * String->Size);
		StringCopy->Size = String->Size;
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

void mas_impl_string_append_cstr(masString** Out, const char* Text, ...)
{
	if (Out && *Out && Text)
	{
		char TempBuf[512] = { 0 };

		va_list Args;
		va_start(Args, Text);
		int32_t Count = vsprintf_s(TempBuf, 512, Text, Args);
		va_end(Args);

		masString *OutString    = *Out;
		uint32_t   RequiredSize = sizeof(masString) + (sizeof(char) * (Count + OutString->Size));
		masString *String       = MAS_IMPL_REALLOC(masString, OutString, RequiredSize);
		if (!String)
		{
			String = MAS_IMPL_MALLOC(masString, RequiredSize);
			MAS_IMPL_ASSERT(String != NULL, "MAS_STRING", "mas_impl_string_append_cstr: allocation failed for -> %s", TempBuf);

			mas_impl_memory_copy(String->Data,                   OutString->Data, sizeof(char) * OutString->Size);
			mas_impl_memory_copy(String->Data + OutString->Size, TempBuf,         sizeof(char) * Count);
			String->Size = OutString->Size + Count;

			MAS_IMPL_FREE(*Out);
			*Out = NULL;
		}
		else
		{
			mas_impl_memory_copy(OutString->Data + OutString->Size, TempBuf, sizeof(char) * Count);
			OutString->Size += Count;
		}
	}
}

void        mas_impl_string_insert_cstr(masString* Out, int32_t At, const char* Text, ...);
void        mas_impl_string_append(masString* Out, const masString* String);
void        mas_impl_string_insert(masString* Out, int32_t At, const masString* String);
int32_t     mas_impl_string_find_first(masString* String, const char* Target);
int32_t     mas_impl_string_find_last(masString* String, const char* Target);
uint64_t    mas_impl_string_hash(const masString* String);
masString* mas_impl_string_substring(const masString* String, int32_t Index, int32_t Size);
bool        mas_impl_string_compare_cstr(const masString* String, const char* Text);
bool        mas_impl_string_compare(const masString* Sting1, const masString* String2);
uint64_t    mas_impl_string_length(const masString* String);
const char* mas_impl_string_cstr(const masString* String);