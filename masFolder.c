#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <stdint.h>


/***********************************************************************************************************
*
************************************************************************************************************/
#define MAS_FOLDER_SIZE 4096
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)


/***********************************************************************************************************
*
************************************************************************************************************/
struct masFile
{
    const char* FileName;
    const char* FilePath;
    const char* FileExtension;
    uint64_t    FileSize;
    uint64_t    NextFileOffset;
}; 

struct masFolder
{
    char *Files;
    char *Path;
    int   PathSize;
};


/***********************************************************************************************************
*
************************************************************************************************************/
static masFolder* mas_folder_internal_allocate(int PathSize)
{
    if(PathSize <= 0 || PathSize >= MAS_FOLDER_SIZE)
        return NULL;

    masFolder* Folder = (masFolder*)malloc(MAS_FOLDER_SIZE);
    if(!Folder)
        return NULL;
    memset(Folder, 0, MAS_FOLDER_SIZE);

    Folder->Path           = MAS_PTR_OFFSET(char, Folder, sizeof(masFolder));
    Folder->Files          = MAS_PTR_OFFSET(char, Folder->Path, PathSize);
    Folder->PathSize       = PathSize;
    Folder->FileCount      = 0;
    Folder->FileMemorySize = MAS_FOLDER_SIZE - (sizeof(masFolder) + PathSize);

    return Folder;
}


/***********************************************************************************************************
*
************************************************************************************************************/
masFolder* mas_folder_current()
{
    DWORD PathSize = GetCurrentDirectory(0, NULL);

    masFolder* Folder = mas_folder_internal_allocate(PathSize);
    if(!Folder)
        return NULL;
    GetCurrentDirectory(Folder->PathSize, Folder->Path);

    return Folder;
}

const char* mas_folder_path(masFolder* Folder)
{
    if(!Folder || Folder->PathSize == 0)
        return NULL;
    return Folder->Path;
}

masFolder* mas_folder_create(const char* FolderPath)
{
    DWORD PathSize = strlen(FolderPath);

    masFolder* Folder = mas_folder_internal_allocate(PathSize);
    if(!Folder)
        return NULL;
    memcpy(Folder->Path, FolderPath, sizeof(char) * PathSize);

    return Folder;
}

void mas_folder_destroy(masFolder** Folder)
{
    if(!Folder || !(*Folder))
        return;
    free(*Folder);
    *Folder = NULL;
}

struct masSearchPath
{
    const char    *Path;
    masSearchPath *Next;
}

struct masSearchContext
{
    char             *SearchPathList;     // Holds all subfolder's paths waiting to be search as masSearchPath    
    char             *TempBuf;            // Used to construct subfolder'path to be added so SubFolderPaths
    HANDLE            FileHandle;         // Current folder being searched;
    WIN32_FIND_DATAA  FindData;           // Current file being inspected;
    int32_t           SearchPathListSize; // Remaining size of SearchPathList
};

static masFolderSearchContext* mas_folder_internal_create_search_context(const char* FolderPath, bool EnableRecursiveSearch)
{
    uint64_t AllocSize = sizeof(masSearchContext);
    if(EnableRecursiveSearch)
    {
        AllocSize += MAS_FOLDER_SIZE;
        AllocSize += 512; // TempBuf
    }
    
    masSearchContext* SearchContext = (masSearchContext*)malloc(AllocSize);
    if(!SearchContext)
        return NULL;
    memset(SearchContext, 0, AllocSize);

    if(EnableRecursiveSearch)
    {
        SearchContext->TempPath       = MAS_PTR_OFFSET(char, SearchContext, sizeof(masSearchContext));
        SearchContext->SubFolderPaths = MAS_PTR_OFFSET(char, SearchContext->TempPath, 512);
    }

    SearchContext->FileHandle = FindFirstFile(FolderPath, &SearchContext->FindData);
    if(SearchContext->FileHandle == INVALID_HANDLE_VALUE)
    {
        free(SearchContext);
        return NULL;
    }

    return SearchContext;
}


masFile* mas_folder_search_for_file(masFolder* Folder, const char* Name, bool EnableRecursiveSearch)
{
    if(!Folder || !Name)
        return NULL;

    masSearchContext* Context = mas_folder_internal_create_search_context(Folder->Path, EnableRecursiveSearch);
    if(!Context)
        return NULL;

    masFile    *File = NULL;
    const char *IgnoreFiles[]   = { ".", ".." };
    int         IgnoreFileCount = sizeof(IgnoreFiles)/sizeof(IgnoreFiles[0]);

    while(1)
    {
        bool bStopSearch = false;
        for(int i = 0; i < IgnoreFileCount; ++i)
        {
            if(strcmp(FindData.cFileName, IgnoreFiles[i]) == 0)
            {
                bStopSearch = true;
                break;
            }
        }   

        if(!bStopSearch)
        {
            if((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && EnableRecursiveSearch)
            {
                sprintf(SubFolderPath, "%s\\%s\\", Folder->Path, FindData.cFileName);

            }
            else
            {

            }
        }

        if(!FindNextFile(FileHandle, &FindData))
        {
            FindClose(FileHandle);
            if(!EnableRecursiveSearch)
                break;           
        }
    };
    
    free(Context);
    Context = NULL;

    return File;
}