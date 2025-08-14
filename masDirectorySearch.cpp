#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

//#include "masDirectorySearch.h"

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
    char     *Path;
    char     *Name;
    char     *Extension;
    uint64_t  NextFileOffset;
};

struct masFoundFiles
{
    uint8_t *Files;
    int32_t  Size;
    int32_t  AllocIdx;
    int32_t  GetIdx;
};

struct masSubFolders
{
    char    PathList[MAS_SUB_FOLDER_COUNT][MAS_PATH_SIZE];
    int32_t AddIdx;
    int32_t GetIdx;
};

struct masDirectorySearch
{
    masSubFolders *SubFolders;
    masFoundFiles  FoundFiles;
    char           Path[MAS_PATH_SIZE];
};


/***********************************************************************************************************
*
************************************************************************************************************/
static const char* mas_internal_win32_error_msg()
{
    static char Msg[512];
    memset(Msg, 0, 512);
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
        GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), Msg, 512, NULL);
    return Msg;
}

static bool mas_interanl_skip_search(const char* FileName)
{
    static const char* IgnoreFiles[]   = { ".", ".." };
    static int         IgnoreFileCount = MAS_ARRAY_SIZE(IgnoreFiles);

    for(int32_t i = 0; i < IgnoreFileCount; ++i)
        if(strcmp(IgnoreFiles[i], FileName) == 0)
            return true;
    
    return false;
}

static void mas_internal_add_sub_folder_path(masSubFolders* SubFolders, const char* SearchingPath, const char* FolderName)
{
    int32_t  SearchPathSize = strlen(SearchingPath) - 1;
    int32_t  FolderNameSize = strlen(FolderName);
    int32_t  AddIdx         = SubFolders->AddIdx;
    char    *SubFolderPath  = SubFolders->PathList[AddIdx];
    if(strlen(SubFolderPath) <= 0)
    {
        memcpy(SubFolderPath, SearchingPath, SearchPathSize);
        //memcpy(SubFolderPath + SearchPathSize, "\\", sizeof(char));
        memcpy(SubFolderPath + SearchPathSize, FolderName, FolderNameSize); 
        memcpy(SubFolderPath + (SearchPathSize + FolderNameSize), "\\*", sizeof(char) * 2);
        SubFolders->AddIdx++;
        if(SubFolders->AddIdx >= MAS_SUB_FOLDER_COUNT)
            SubFolders->AddIdx = 0;
    }
    else
        printf("[ FAILED ]: MAS_DIRECTORY_SEARCH -> interanl sub folder path list is full to add\n\t\t%s\\%s\n", SearchingPath, FolderName);
}

static bool mas_internal_get_sub_folder_path(masSubFolders* SubFolders, char* OutSearchingPath)
{
    int32_t  GetIdx            = SubFolders->GetIdx;
    char    *SubFolderPath     = SubFolders->PathList[GetIdx];
    int32_t  SubFolderPathSize = strlen(SubFolderPath);
    if(SubFolderPathSize > 0)
    {
        memset(OutSearchingPath, 0, MAS_PATH_SIZE);
        memcpy(OutSearchingPath, SubFolderPath, SubFolderPathSize);
        memset(SubFolderPath, 0, SubFolderPathSize);

        SubFolders->GetIdx++;
        if(SubFolders->GetIdx >= MAS_SUB_FOLDER_COUNT)
            SubFolders->GetIdx = 0;
        
        return true;
    }

    return false;
}


static void mas_internal_found_file_add(masFoundFiles* FoundFiles, const char* FilePath, const char* FileName, const char* FileExtension)
{
    int32_t FilePathSize  = strlen(FilePath)      - 1;
    int32_t NameSize      = strlen(FileName)      + 1; // null terminator
    int32_t ExtensionSize = strlen(FileExtension) + 1; // null terminator
    int32_t PathSize      = (FilePathSize + (NameSize - 1) + (ExtensionSize - 1)) + 1; //

    int32_t RequiredSize = sizeof(masFile) + PathSize + NameSize + ExtensionSize; 
    if(RequiredSize >= (MAS_FILES_BUFFER_SIZE - FoundFiles->AllocIdx))
    {
        printf("[ FAILED ]: MAS_DIRECTORY_SEARCH -> adding found file, internal buffer is full for ( %s\\%s.%s)\n", FilePath, FileName, FileExtension);
        return;
    }

    masFile* File = MAS_PTR_OFFSET(masFile, FoundFiles->Files, FoundFiles->AllocIdx);
    FoundFiles->AllocIdx += RequiredSize;

    File->Path           = MAS_PTR_OFFSET(char, File,       sizeof(masFile));
    File->Name           = MAS_PTR_OFFSET(char, File->Path, PathSize);
    File->Extension      = MAS_PTR_OFFSET(char, File->Name, NameSize);
    File->NextFileOffset = RequiredSize;

    
    memcpy(File->Path,                                 FilePath,      FilePathSize);
    memcpy(File->Path + FilePathSize,                  FileName,      NameSize - 1);
    memcpy(File->Path + (FilePathSize + NameSize - 1), FileExtension, ExtensionSize - 1);

    memcpy(File->Name,      FileName,      NameSize);
    memcpy(File->Extension, FileExtension, ExtensionSize);
}

static bool mas_internal_has_extension(const char* FileName)
{
    int32_t FileNameSize = strlen(FileName);
    for(int32_t i = FileNameSize - 1; i >= 0; --i)
        if(FileName[i] == '.')
            return true;
    return false;
}

static void mas_internal_process_found_file(masFoundFiles* FoundFiles, const char* SearchingPath, const char* FileName, const char** Targets, int32_t TargetCount)
{
    char Name[MAS_PATH_SIZE];
    char Extension[MAS_PATH_SIZE];

    memset(Extension, 0, MAS_PATH_SIZE);
    memset(Name,      0, MAS_PATH_SIZE);

    int32_t FileNameSize    = strlen(FileName);
    int32_t ExtensionOffset = 0;
    for(; ExtensionOffset < FileNameSize; ++ExtensionOffset)
        if(FileName[ExtensionOffset] == '.')
            break;
    int32_t ExtensionSize = FileNameSize - ExtensionOffset;
    int32_t NameSize      = ExtensionOffset;
    memcpy(Name,      FileName,                   NameSize);
    memcpy(Extension, FileName + ExtensionOffset, ExtensionSize);

    for(int32_t i = 0; i < TargetCount; ++i)
    {
        const char* Target = Targets[i];
        if(Target[0] == '.')
        {
            if(strcmp(Extension, Target) == 0)
                mas_internal_found_file_add(FoundFiles, SearchingPath, Name, Extension);
        }
        else if(!mas_internal_has_extension(Target))
        {
            if(strcmp(Name, Target) == 0)
                mas_internal_found_file_add(FoundFiles, SearchingPath, Name, Extension);
        }
        else
        {
            if(strcmp(FileName, Target) == 0)
                mas_internal_found_file_add(FoundFiles, SearchingPath, Name, Extension);
        }
    }
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

    memcpy(DirectorySearch->Path, Path, PathLen);
    DirectorySearch->SubFolders          = MAS_PTR_OFFSET(masSubFolders, DirectorySearch,             sizeof(masDirectorySearch));
    DirectorySearch->FoundFiles.Files    = MAS_PTR_OFFSET(uint8_t,       DirectorySearch->SubFolders, sizeof(masSubFolders));
    DirectorySearch->FoundFiles.Size     = MAS_FILES_BUFFER_SIZE;
    DirectorySearch->FoundFiles.AllocIdx = 0;

    return DirectorySearch;
}

void mas_directory_search_destroy(masDirectorySearch** DirectorySearch)
{
    if(!DirectorySearch || !(*DirectorySearch))
        return;
    free(*DirectorySearch);
    *DirectorySearch = NULL;
}

masFoundFiles* mas_directory_search_run(masDirectorySearch* DirectorySearch, const char** Targets, int32_t TargetCount)
{
    if(!DirectorySearch || !Targets || TargetCount <= 0)
        return NULL;

    int32_t SearchingPathSize = strlen(DirectorySearch->Path);
    if(SearchingPathSize <= 0)
        return NULL;

    WIN32_FIND_DATAA FindData;
    memset(&FindData, 0, sizeof(WIN32_FIND_DATAA));

    char SearchingPath[MAS_PATH_SIZE];
    memset(SearchingPath, 0, MAS_PATH_SIZE);
    memcpy(SearchingPath, DirectorySearch->Path, SearchingPathSize);

    //HANDLE      FileHandle    = FindFirstFileExA(SearchingPath, FindExInfoBasic, &FindData, FindExSearchMaxSearchOp, NULL, FIND_FIRST_EX_LARGE_FETCH);
    HANDLE      FileHandle    = FindFirstFileA(SearchingPath, &FindData);
    if(FileHandle == INVALID_HANDLE_VALUE)
    {
        printf("[ FAILED ]: MAS_DIRECTORY_SEARCH -> FindFirstFileExA -> %s\n", mas_internal_win32_error_msg());
        return NULL;
    }

    
    while(1)
    {
        if(!mas_interanl_skip_search(FindData.cFileName))
        {
            if(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                mas_internal_add_sub_folder_path(DirectorySearch->SubFolders, SearchingPath, FindData.cFileName);
            else
                mas_internal_process_found_file(&DirectorySearch->FoundFiles, SearchingPath, FindData.cFileName, Targets, TargetCount);
        }

        if(!FindNextFileA(FileHandle, &FindData))
        {
            FindClose(FileHandle);
 
            if(!mas_internal_get_sub_folder_path(DirectorySearch->SubFolders, SearchingPath))
                break;

            //FileHandle = FindFirstFileExA(SearchingPath, FindExInfoBasic, &FindData, FindExSearchMaxSearchOp, NULL, FIND_FIRST_EX_LARGE_FETCH);
            FileHandle = FindFirstFileA(SearchingPath, &FindData);
            if(FileHandle == INVALID_HANDLE_VALUE)
            {
                printf("[ FAILED ]: MAS_DIRECTORY_SEARCH -> FindFirstFileExA -> %s\n", mas_internal_win32_error_msg());
                break;
            }
        }
    }

    return &DirectorySearch->FoundFiles;
}

masFile* mas_found_files_next(masFoundFiles* FoundFiles)
{
    if(!FoundFiles)
        return NULL;

    if(FoundFiles->GetIdx >= FoundFiles->AllocIdx)
        return NULL;
    masFile *File       = MAS_PTR_OFFSET(masFile, FoundFiles->Files, FoundFiles->GetIdx);
    FoundFiles->GetIdx += File->NextFileOffset;

    return File;
}





/**************************************
* API TEST
***************************************/
int main(int argc, const char** argv)
{
    char Path[MAS_PATH_SIZE];
    memset(Path, 0, MAS_PATH_SIZE);
    int32_t PathSize = GetCurrentDirectoryA(MAS_PATH_SIZE, Path);
    Path[PathSize++] = '\\';
    Path[PathSize++] = '*';

    masDirectorySearch* AssetSearchDirectory = mas_directory_search_create(Path);

    const char    *SearchTarget[]    = { "BackgroundAmbient", ".png", "IntroVideo", ".tga", ".jpeg", ".jpg", ".obj", ".exe", ".c", ".h", ".ilk", ".pdb", ".bat"};
    int32_t        SearchTargetCount = (sizeof(SearchTarget)/sizeof(SearchTarget[0]));
    masFoundFiles *FoundFiles        = mas_directory_search_run(AssetSearchDirectory, SearchTarget, SearchTargetCount);

    for(masFile* File = mas_found_files_next(FoundFiles); File; File = mas_found_files_next(FoundFiles))
    {
        printf("FILE_INFO:\n");
        printf("\tPATH:      %s\n", File->Path);
        printf("\tNAME:      %s\n", File->Name);
        printf("\tEXTENSION: %s\n", File->Extension);
    }

    mas_directory_search_destroy(&AssetSearchDirectory);
}