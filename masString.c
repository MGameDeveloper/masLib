#include <assert.h>


/************************************************************************************************************************
* 
*************************************************************************************************************************/
#define MAS_INVALID_INDEX -1
#define MAS_TEMP_BUF_SIZE 512
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((unsigned char*)ptr) + offset)

#define MAS_MALLOC(type, size) (type*)malloc(size)
#define MAS_FREE(ptr) free(ptr)

#if defined(UNICODE) || (_UNICODE)
    #define MAS_STRLEN(T) lstrlenW(T)
    #define MAS_CHAR(C) L##C
#else 
    #define MAS_STRLEN(T) strlen(T)
    #define MAS_CHAR(C) C
#endif


/************************************************************************************************************************
* 
*************************************************************************************************************************/
struct masString
{
    masChar *Data;
    int      Size;
    int      Capacity;
};


/************************************************************************************************************************
* 
*************************************************************************************************************************/
static masString* mas_internal_create_string_by_size(int TextSize)
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

static int mas_internal_calculate_text_size(const masChar* Text)
{
    if(!Text)
        return 0;
    int Size = 0;
    while(*Text++ != MAS_CHAR('\0'))
        Size++;
    return Size;
}


/************************************************************************************************************************
* 
*************************************************************************************************************************/
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

masString* mas_string_create_substring(masString* String, int Offset, int Size)
{
    if(!String)
        return NULL;
    if((Offset < 0 || Offset >= String->Size) || (Size < 0 || Size >= String->Size))
        return NULL;
    if(Offset + Size > String->Size)
        return NULL;

    masString* SubString = mas_string_internal_create(Size);
    memcpy(SubString->Data, String->Data + Offset, sizeof(masChar) * Size);
    SubString->Size     = Size;
    SubString->Capacity = Size;

    return SubString;
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

        memcpy(TempBuf,            Text,         sizeof(masChar) * TextSize);
        memcpy(TempBuf + TextSize, String->Data, sizeof(masChar) * String->Size);

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

        memcpy(TempBuf,                String->Data, sizeof(masChar) * String->Size);
        memcpy(TempBuf + String->Size, Text,         sizeof(masChar) * TextSize);

        memset(String->Data,       0, sizeof(masChar) * String->Size);
        memcpy(String->Data, TempBuf, sizeof(masChar) * RequiredSize);
        String->Size = RequiredSize;
    }
}

void mas_string_insert_at(masString** StringRef, int Offset, const masChar* Text)
{
    if(!StringRef || !(*StringRef) || Offset < 0 || !Text)
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

        memcpy(OutString->Data,                       String->Data,          sizeof(masChar) * Offset);
        memcpy(OutString->Data + Offset,              Text,                  sizeof(masChar) * TextSize);
        memcpy(OutString->Data + (Offset + TextSize), String->Data + Offset, sizeof(masChar) * (String->Size - Offset));

        mas_string_destroy(StringRef);
        *StringRef = OutString;
    }
    else
    {
        assert(RequiredSize < MAS_TEMP_BUF_SIZE && "TempBuf is small for internal operation of strings, consider increasing MAS_TEMP_BUF_SIZE macro.");

        masChar TempBuf[MAS_TEMP_BUF_SIZE];
        memset(TempBuf, 0, sizeof(masChar) * MAS_TEMP_BUF_SIZE);

        memcpy(TempBuf,                       String->Data,          sizeof(masChar) * Offset);
        memcpy(TempBuf + Offset,              Text,                  sizeof(masChar) * TextSize);
        memcpy(TempBuf + (Offset + TextSize), String->Data + Offset, sizeof(masChar) * (String->Size - Offset));

        memset(String->Data,       0, sizeof(masChar) * String->Size);
        memcpy(String->Data, TempBuf, sizeof(masChar) * RequiredSize);
        String->Size = RequiredSize;
    }
}

int mas_string_find_first(masString* String, masChar Char)
{
    if(!String || Char == MAS_CHAR('\0'))
        return MAS_INVALID_INDEX;

    for(int i = 0; i < String->Size; ++i)
        if(String->Data[i] == Char)
            return i;

    return MAS_INVALID_INDEX;
}

int mas_string_find_last(masString* String, masChar Char)
{
    if(!String || Char == MAS_CHAR('\0'))
        return MAS_INVALID_INDEX;
    
    for(int i = String->Size - 1; i >= 0; --i)
        if(String->Data[i] == Char)
            return i;
    
    return MAS_INVALID_INDEX;
}

int mas_string_size(masString* String)
{
    if(!String)
        return 0;
    return String->Size;
}

bool mas_string_is_equal(masString* String1, masString* String2)
{
    if(!String1 || !String2)
        return false;

    if(String1->Size != String2->Size)
        return false;

    int Size  = String1->Size;
    int Index = 0;
    while(Index < Size)
    {
        if(String1->Data[Index] != String2->Data[Index])
            return false;
        Index++;
    }

    return true;
}










/**************************************************************************************************************************
* EXAMPLE
***************************************************************************************************************************/
//int main(int argc, const char** argv)
//{
//    masString* IntroMsg = mas_string_create(L"Glad you choosen to be on of the");
//    mas_string_insert_last(&IntroMsg, L" immortals who loves to be on the verge of destruction.");
//    mas_string_insert_first(&IntroMsg, L"Welcome to the masEngine, we are ");
//    mas_string_insert_replace(&IntroMsg, L"Glad you", L"glad you've");
//    mas_string_remove(Intro, "immortals");
//
//    return 0;
//}


