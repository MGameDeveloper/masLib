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


masString* mas_string_create(const masChar* Text)
{
    int TextSize   = mas_strlen(Text);
    if(TextSize <= 0)
        return NULL;
    else
        TextSize++; // NULL terminator need to check if we need it

    int HeaderSize = sizeof(masString);
    int AllocSize  = HeaderSize + (sizeof(masChar) * TextSize);
    masString* String = MAS_MALLOC(masString, AllocSize);
    if(!String)
        return NULL;
    memset(String, 0, AllocSize);

    String->Data     = MAS_PTR_OFFSET(masChar, String, HeaderSize);
    String->Size     = TextSize;
    String->Capacity = TextSize;

    return String;
}

void mas_string_destroy(masString** String)
{
    if(!String || !(*String))
        return;
    MAS_FREE(*String);
    *String = NULL;
}

void mas_string_insert_first(masString* String, const masChar* Text)
{
    if(!String)
        return;

    int TextSize = MAS_STRLEN(Text);
    if(TextSize <= 0)
        return;
    else
        TextSize++; // Null terminator may need to check we may not need it

    int RequiredSize = TextSize + String->Size;
    assert(RequiredSize < MAS_TEMP_BUF_SIZE && "Temporary buffer is smaller than the data being worked on.");

    masChar TempText[MAS_TEMP_BUF_SIZE];
    memset(TempText, 0, sizeof(masChar) * MAS_TEMP_BUF_SIZE);

    int CopyOffset = TextSize;
    memcpy(TempText,              Text,         sizeof(masChar) * TextSize);
    memcpy(TempText + CopyOffset, String->Data, sizeof(masChar) * String->Size);

    if(RequiredSize < String->Capacity)
    {
        
    }
    else
    {

    }
}

void mas_string_insert_last(masString* String, const masChar* Text)
{
    if(!String)
        return;

    int TextSize = MAS_STRLEN(Text);
    if(TextSize <= 0)
        return;
    else
        TextSize++; // Null terminator may need to check we may need it    
}