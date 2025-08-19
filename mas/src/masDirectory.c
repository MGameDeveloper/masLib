#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <string.h>

#include "masTypes.h"

/***************************************************************************************************************************
* 
****************************************************************************************************************************/
#define MAS_MEM_SIZE_MB(x) (1024llu * 1024llu * x##llu)
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_FOLDER_BUF_SIZE MAS_MEM_SIZE_MB(1)
#define MAS_FILE_BUF_SIZE   MAS_MEM_SIZE_MB(1)
#define MAS_PATH_LEN(path)  mas_internal_path_len(path)


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
typedef struct _masFolder
{
    const masChar* Path;
    int32_t        PathSize;
} masFolder;

typedef struct _masFile
{
    const masChar* Path;
    const masChar* Name;
    const masChar* Extension;
    uint64_t       Size;
} masFile;

typedef struct _masFolderBuf
{
    uint8_t Folders[MAS_FOLDER_BUF_SIZE];
    int32_t AllocIdx;
    int32_t QueryIdx;
    int32_t FolderCount;
} masFolderBuf;

typedef struct _masFileBuf
{
    uint8_t Files[MAS_FILE_BUF_SIZE];
    int32_t AllocIdx;
    int32_t QueryIdx;
    int32_t FileCount;
} masFileBuf;

typedef struct _masDirecotry
{
    masFolderBuf FolderBuf;
    masFileBuf   FileBuf;
    int32_t      WorkPathSize;
    masChar      WorkPath[MAX_PATH];
} masDirectory;


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
static masDirectory* Directory = NULL;


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
int32_t mas_internal_path_len(const masChar* Path)
{
    const masChar* LocalPath = Path;
    while(*LocalPath) 
        ++LocalPath;
    return (LocalPath - Path);
}

void mas_internal_directory_folder_add(const masChar* FolderPath)
{
    if(!Directory)
        return;

    int32_t PathSize  = MAS_PATH_LEN(FolderPath);
    if(PathSize == 0)
        return;
    PathSize++; // NULL Terminator

    uint64_t      RequiredSize = sizeof(masFolder) + PathSize;
    masFolderBuf *FolderBuf    = &Directory->FolderBuf;
    if(FolderBuf->AllocIdx + RequiredSize >= MAS_FOLDER_BUF_SIZE)
        return false;
    
    masFolder* Folder = MAS_PTR_OFFSET(masFolder, FolderBuf->Folders, FolderBuf->AllocIdx);
    Folder->PathSize  = PathSize;
    Folder->Path      = MAS_PTR_OFFSET(masChar, Folder, sizeof(masFolder));
    memcpy(Folder->Path, FolderPath, sizeof(masChar) * PathSize);
}

const masFolder* mas_internal_directory_folder_get_next()
{
    if(!Directory)
        return NULL;

    masFolderBuf* FolderBuf = &Directory->FolderBuf;
    if(FolderBuf->QueryIdx >= FolderBuf->AllocIdx)
        return NULL;
    
    masFolder* Folder = MAS_PTR_OFFSET(masFolder, FolderBuf->Folders, FolderBuf->QueryIdx);
    FolderBuf->QueryIdx += sizeof(masFolder) + Folder->PathSize;
    
    return Folder;
}


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
bool mas_impl_directory_init()
{
    if(Directory)
        return true;

    Directory = (masDirectory*)malloc(sizeof(masDirectory));
    if(!Directory)
        return false;
    memset(Directory, 0, sizeof(masDirectory));

    Directory->WorkPathSize = GetModuleFileName(NULL, Directory->WorkPath, MAX_PATH);

    return true;
}

void mas_impl_directory_deinit()
{
    if(!Directory)
        return;
    free(Directory);
    Directory = NULL;
}

const masChar* mas_impl_directory_find_folder(const masChar* DirectoryPath, const masChar* FolderName)
{
    
}