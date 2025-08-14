#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <stdint.h>


/***********************************************************************************************************
*
************************************************************************************************************/
#define MAS_PATH_SIZE 256
#define MAS_FILES_BUFFER_SIZE 4096
#define MAS_SUB_FOLDER_COUNT  1024
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
    uint64_t    Size;
    uint64_t    NextFileOffset;
};

struct masFoundFiles
{
    uint8_t *Files;
    int32_t  Size;
    int32_t  AllocIdx;
};

struct masSubFolders
{
    char    PathList[MAS_SUB_FOLDER_COUNT][MAS_PATH_SIZE];
    int32_t AddIdx;
    int32_t GetIdx;
}

struct masDirectorySearch
{
    masSubFolders *SubFolders;
    masFoundFiles  FoundFiles;
    bool           EnableRecursiveSearch;
    char           Path[MAS_PATH_SIZE];
};


/***********************************************************************************************************
*
************************************************************************************************************/
static bool mas_interanl_skip_search(const char* FileName)
{
    static const char* IgnoreFiles[]   = { ".", ".." };
    static int         IgnoreFileCount = MAS_ARRAY_SIZE(IgnoreFiles);

    for(int32_t i = 0; i < IgnoreFileCount; ++i)
        if(strcmp(IgnoreFiles[i], FindData.CFileName) == 0)
            return true;
    
    return false;
}

static void mas_internal_add_sub_folder_path(masSubFolders* SubFolders, const char* SearchingPath, const char* FolderName)
{

}

static const char* mas_internal_get_sub_folder_path(masSubFolders* SubFolders)
{

}

static void mas_internal_process_found_file(masFoundFiles* FoundFiles, const char* SearchingPath, const char* FileName, const char** Targets, int32_t TargetCount)
{

}


/***********************************************************************************************************
*
************************************************************************************************************/
masDirectorySearch* mas_directory_search_create(const char* Path)
{
    int32_t PathLen = strlen(Path);
    if(PathLen <= 0)
        return NULL;

    uint64_t AllocSize = sizeof(masDirectorySearch) + sizeof(masSubFolders) + MAS_FILES_BUFFER_SIZE;
    masDirectorySearch* DirectorySearch = (masDirectorySearch*)malloc(AllocSize);
    if(!DirectorySearch)
        return NULL;
    memset(DirectorySearch, 0, AllocSize);

    memcpy(DirectorySearch->Path, 0, PathLen);
    DirectorySearch->SubFolders             = MAS_PTR_OFFSET(masSubFolders, DirectorySearch,             sizeof(masDirectorySearch));
    DirectorySearch->FoundFiles.Files       = MAS_PTR_OFFSET(masFoundFiles, DirectorySearch->SubFolders, sizeof(masSubFolders));
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

masFile* mas_directory_search_find(masDirectorySearch* DirectorySearch, const char** Targets, int32_t TargetCount)
{
    if(!DirectorySearch || !Targets || TargetCount <= 0)
        return NULL;

    WIN32_FIND_DATA FindData;
    memset(&FindData, 0, sizeof(FindData));

    const char *SearchingPath = DirectorySearch->Path;
    HANDLE      FileHandle    = FindFirstFileExA(SearchingPath, FindExInfoBasic, &FindData, FindExSearchMaxSearchOp, NULL, FIND_FIRST_EX_LARGE_FETCH);
    if(FileHandle == INVALID_HANDLE_VALUE)
        return NULL;

    
    while(1)
    {
        if(!mas_interanl_skip_search(FindData.cFileName))
        {
            if(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                mas_internal_add_sub_folder_path(SearchingPath, FindData.cFileName);
            else
                mas_internal_process_found_file(DirectorySearch->FoundFiles, SearchingPath, FindData.cFileName, Targets, TargetCount);
        }

        if(!FindNextFileA(FileHandle, &FindData))
        {
            CloseHandle(FileHandle);

            SearchingPath = mas_internal_get_sub_folder_path(DirectorySearch->SubFolders);
            if(!SearchingPath)
                break;

            FileHandle = FindFirstFileExA(SearchingPath, FindExInfoBasic, &FindData, FindExSearchMaxSearchOp, NULL, FIND_FIRST_EX_LARGE_FETCH);
            if(FileHandle == INVALID_HANDLE_VALUE)
                break;
        }
    }

    return masFile*;
}

masFile* mas_directory_search_find_next(masDirectorySearch* DirectorySearch)
{

}