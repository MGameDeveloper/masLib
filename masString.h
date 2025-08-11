#include <assert.h>

#define MAS_TEMP_BUF_SIZE 512
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((unsigned char*)ptr) + offset)

#define MAS_MALLOC(type, size) (type*)malloc(size)
#define MAS_FREE(ptr) free(ptr)

#if defined(UNICODE) || (_UNICODE)
    typedef wchar_t masChar
    #define MAS_STRLEN(T) lstrlenW(T)
#else 
    typedef char masChar;
    #define MAS_STRLEN(T) strlen(T)
#endif


struct masString
{
    masChar *Data;
    int      Size;
    int      Capacity;
};

masString* mas_string_internal_create(int TextSize)
{
    if(TextSize <= 0)
        return NULL;
    else
        TextSize++; // null terminator need to be proven we may not need it

    int AllocSize = sizeof(masString) + (sizeof(masChar) * TextSize);
    masString* String = (masString*)malloc(AllocSize);
    if(!String)
        return NULL;
    memset(String, 0, AllocSize);
    String->Data     = MAS_PTR_OFFSET(masChar, String, sizeof(masString));
    String->Size     = TextSize;
    String->Capacity = TextSize;

    return String;
}

masString* mas_string_create(const masChar* Text)
{
    masString* String = mas_string_internal_create(MAS_STRLEN(Text));
    if(!String)
        return NULL;
    int CopySize = String->Size - 1;
    memcpy(String->Data, Text, sizeof(masChar) * CopySize);

    return String;
}

void mas_string_destroy(masString** String)
{
    if(!String || !(*String))
        return;
    MAS_FREE(*String);
    *String = NULL;
}

void mas_string_insert_first(masString** StringRef, const masChar* Text)
{
    if(!String || !(*String))
        return;

    int TextSize = MAS_STRLEN(Text);
    if(TextSize <= 0)
        return;
    else
        TextSize++; // Null terminator may need to check we may not need it

    masString *String       = *StringRef;
    int        RequiredSize = String->Size + TextSize;
    if(RequiredSize >= String->Capacity)
    {
        masString *OutString = mas_string_internal_create(RequiredSize);
        assert(OutString != NULL && "Failed to internally resize string buffer");

        memcpy(OutString->Data,            Text,         sizeof(masChar) * TextSize);
        memcpy(OutString->Data + TextSize, String->Data, sizeof(masChar) * String->Size);

        mas_string_destroy(StringRef);
        *StringRef = OutString;
    }
    else
    {
        assert(RequiredSize < MAS_TEMP_BUF_SIZE && "TempBuf is small for internal operation of strings, consider increasing MAS_TEMP_BUF_SIZE macro.");

        masChar TempBuf[MAS_TEMP_BUF_SIZE];
        memset(TempBuf, 0, sizeof(masChar) * MAS_TEMP_BUF_SIZE);

        memcpy(OutString->Data,            Text,         sizeof(masChar) * TextSize);
        memcpy(OutString->Data + TextSize, String->Data, sizeof(masChar) * String->Size);

        memset(String->Data,       0, sizeof(masChar) * String->Size);
        memcpy(String->Data, TempBuf, sizeof(masChar) * RequiredSize);
        String->Size = RequiredSize;
    }
}

void mas_string_insert_last(masString** StringRef, const masChar* Text)
{
    if(!String)
        return;

    int TextSize = MAS_STRLEN(Text);
    if(TextSize <= 0)
        return;
    else
        TextSize++; // Null terminator may need to check we may need it  
    
    masString *String       = *StringRef;
    int        RequiredSize = String->Size + TextSize;
    if(RequiredSize >= String->Capacity)
    {
        masString *OutString = mas_string_internal_create(RequiredSize);
        assert(OutString != NULL && "Failed to internally resize string buffer");

        memcpy(OutString->Data,                String->Data, sizeof(masChar) * String->Size);
        memcpy(OutString->Data + String->Size, Text,         sizeof(masChar) * TextSize);

        mas_string_destroy(StringRef);
        *StringRef = OutString;
    }
    else
    {
        assert(RequiredSize < MAS_TEMP_BUF_SIZE && "TempBuf is small for internal operation of strings, consider increasing MAS_TEMP_BUF_SIZE macro.");

        masChar TempBuf[MAS_TEMP_BUF_SIZE];
        memset(TempBuf, 0, sizeof(masChar) * MAS_TEMP_BUF_SIZE);

        memcpy(OutString->Data,                String->Data, sizeof(masChar) * String->Size);
        memcpy(OutString->Data + String->Size, Text,         sizeof(masChar) * TextSize);

        memset(String->Data,       0, sizeof(masChar) * String->Size);
        memcpy(String->Data, TempBuf, sizeof(masChar) * RequiredSize);
        String->Size = RequiredSize;
    }
}

void mas_string_insert_at(masString** StringRef, int Offset, const masChar* Text)
{

}












/**************************************************************************************************************************
* EXAMPLE
***************************************************************************************************************************/
int main(int argc, const char** argv)
{
    masString* IntroMsg = mas_string_create(L"Glad you choosen to be on of the");
    mas_string_insert_last(&IntroMsg, L" immortals who loves to be on the verge of destruction.");
    mas_string_insert_first(&IntroMsg, L"Welcome to the masEngine, we are ");
    mas_string_insert_replace(&IntroMsg, L"Glad you", L"glad you've");
    mas_string_remove(Intro, )
    return 0;
}
