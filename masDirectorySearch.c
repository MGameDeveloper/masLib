#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <stdint.h>


/***********************************************************************************************************
*
************************************************************************************************************/
#define MAS_PATH_SIZE 256
#define MAS_FILES_BUFFER_SIZE 4096
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_ARRAY_SIZE(array)             (sizeof(array)/sizeof(array[0]))

/***********************************************************************************************************
*
************************************************************************************************************/
struct masFile
{
    const char* Path;
    const char* Name;
    const char* Extension;
};

struct masFoundFiles
{
    uint8_t *Files;
    int32_t  Size;
    int32_t  AllocIdx;
};

struct masDirectorySearch
{
    masFoundFiles FoundFiles;
    char          Path[MAS_PATH_SIZE];
};


/***********************************************************************************************************
*
************************************************************************************************************/


/***********************************************************************************************************
*
************************************************************************************************************/
masDirectorySearch* mas_directory_search_create(const char* Path, bool EnableRecursiveSearch)
{
    int32_t PathLen = strlen(Path);
    if(PathLen <= 0)
        return NULL;

    uint64_t AllocSize = sizeof(masDirectorySearch) + MAS_FILES_BUFFER_SIZE;
    masDirectorySearch* DirectorySearch = (masDirectorySearch*)malloc(AllocSize);
    if(!DirectorySearch)
        return NULL;
    memset(DirectorySearch, 0, AllocSize);

    memcpy(DirectorySearch->Path, 0, PathLen);
    DirectorySearch->FoundFiles.Files       = MAS_PTR_OFFSET(masFoundFiles, DirectorySearch, sizeof(masDirectorySearch));
    DirectorySearch->SearchBuffer->Size     = MAS_SEARCH_BUFFER_SIZE;
    DirectorySearch->SearchBuffer->AllocIdx = 0;

    return DirectorySearch;
}

void mas_directory_search_destroy(masDirectorySearch** DirectorySearch)
{
    if(!DirectorySearch || !(*DirectorySearch))
        return;
    free(*DirectorySearch);
    *DirectorySearch = NULL;
}

masFile* mas_directory_search_find_file(masDirectorySearch* DirectorySearch, const char** Targets, int32_t TargetCount)
{
    if(!DirectorySearch || !Targets || TargetCount <= 0)
        return NULL;

    WIN32_FIND_DATA FindData;
    memset(&FindData, 0, sizeof(FindData));

    HANDLE FileHandle = FindFirstFileExA(DirectorySearch->Path, FindExInfoBasic, &FindData, FindExSearchMaxSearchOp, NULL, FIND_FIRST_EX_LARGE_FETCH);
    if(FileHandle == INVALID_HANDLE_VALUE)
        return NULL;

    const char* IgnoreFiles[]   = { ".", ".." };
    int         IgnoreFileCount = MAS_ARRAY_SIZE(IgnoreFiles);

    while(1)
    {
        bool bSkip = false;
        for(int32_t i = 0; i < TargetCount; ++i)
        {
            if(strcmp(IgnoreFiles[i], FindData.CFileName) == 0)
            {
                bSkip = true;
                break;
            }
        }

        if(!bSkip)
        {

        }


    }
}

masFile* mas_directory_search_next_file(masDirectorySearch* DirectorySearch)
{

}