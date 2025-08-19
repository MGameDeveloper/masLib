#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <string.h>

#include "masTypes.h"

/***************************************************************************************************************************
* 
****************************************************************************************************************************/
#define MAS_MEM_SIZE_MB(x)                (1024llu * 1024llu * x##llu)
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_FOLDER_BUF_SIZE                MAS_MEM_SIZE_MB(1)
#define MAS_FILE_BUF_SIZE                  MAS_MEM_SIZE_MB(1)
#define MAS_TEXT_LEN(path)                 mas_internal_directory_path_len(path)
#define MAS_PATH_SIZE                      512


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
    masChar      WorkPath[MAS_PATH_SIZE];
} masDirectory;


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
static masDirectory* Directory = NULL;


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
static int32_t mas_internal_directory_path_len(const masChar* Path)
{
    const masChar* LocalPath = Path;
    while(*LocalPath) 
        ++LocalPath;
    return (LocalPath - Path);
}

static int32_t mas_internal_directory_file_extension_index(const masChar* FileName, int32_t FileNameSize)
{
    for(int32_t i = FileNameSize - 1; i >= 0; --i)
        if(FileName[i] == _T('.'))
            return i;
    return -1;
}

static void mas_internal_directory_folder_add(const masChar* Path, const masChar* Name)
{
    int32_t PathSize = MAS_TEXT_LEN(Path);
    int32_t NameSize = MAS_TEXT_LEN(Name);
    if(PathSize <= 0 || NameSize <= 0)
        return;
    int32_t FinalPathSize = PathSize + NameSize + 1; // + 1 NULL TERMINATOR

    uint64_t      RequiredSize = sizeof(masFolder) + FinalPathSize;
    masFolderBuf *FolderBuf    = &Directory->FolderBuf;
    if(FolderBuf->AllocIdx + RequiredSize >= MAS_FOLDER_BUF_SIZE)
    {
        printf("[ ERROR ]: MAS_DIRECTORY_ADD_FOLDER -> internal buffer is full\n");
        return false;
    }
    
    masFolder* Folder = MAS_PTR_OFFSET(masFolder, FolderBuf->Folders, FolderBuf->AllocIdx);
    Folder->PathSize  = FinalPathSize - 1; // need to check if -1 is correct
    Folder->Path      = MAS_PTR_OFFSET(masChar, Folder, sizeof(masFolder));
    memcpy(Folder->Path, FolderPath, sizeof(masChar) * Folder->PathSize);

    FolderBuf->AllocIdx += RequiredSize;
    FolderBuf->FolderCount++;
}

static const bool mas_internal_directory_folder_get_next(masChar* Path)
{
    masFolderBuf* FolderBuf = &Directory->FolderBuf;
    if(FolderBuf->QueryIdx >= FolderBuf->AllocIdx)
        return false;
    
    masFolder* Folder = MAS_PTR_OFFSET(masFolder, FolderBuf->Folders, FolderBuf->QueryIdx);
    FolderBuf->QueryIdx += sizeof(masFolder) + Folder->PathSize;
    
    memset(Path, 0, MAS_PATH_SIZE);
    memcpy(Path, Folder->Path, sizeof(masChar) * Folder->PathSize);

    return true;
}

static void mas_internal_directory_file_add(const masChar* FilePath, const masChar* FileName)
{
    int32_t FilePathSize = MAS_TEXT_LEN(FilePath);
    int32_t FileNameSize = MAS_TEXT_LEN(FileName);
    if(FilePathSize <= 0 || FileNameSize <= 0)
        return;
    int32_t FileExtensionIdx     = mas_internal_directory_file_extension_index(FileName);
    int32_t FileExtensionSize    = FileNameSize - FileExtensionIdx;
    int32_t CompleteFilePathSize = FilePathSize + FileNameSize + 2; // 2 -> 1 for '\' seperator and 1 for the end as null terminator

    int32_t RequiredSize = sizeof(masFile) + (CompleteFilePathSize + (FileNameSize + 1) + (FileExtensionSize + 1)); // + 1 is for null terminator
    masFileBuf* FileBuf = &Directory->FileBuf;
    if(FileBuf->AllocIdx + RequiredSize >= MAS_FILE_BUF_SIZE)
    {
        printf("[ ERROR ]: MAS_DIRECTORY_ADD_FILE -> internal buffer is full\n");
        return;
    }
    
    masFile* File   = MAS_PTR_OFFSET(masFile, FileBuf->Files, FileBuf->AllocIdx);
    File->Path      = MAS_PTR_OFFSET(masChar, File, sizeof(masFile));
    File->Name      = MAS_PTR_OFFSET(masChar, File->Path, CompleteFilePathSize);
    File->Extension = MAS_PTR_OFFSET(masChar, File->Name, FileNameSize);
    
    memcpy(File->Path,                    FilePath,                    FilePathSize);
    memcpy(File->Path + FilePathSize,     "\\",                        sizeof(masChar));
    memcpy(File->Path + FilePathSize + 1, FileName,                    FileNameSize); // + 1 for the above "\\"
    memcpy(File->Name,                    FileName,                    FileNameSize);
    memcpy(File->Extension,               FileName + FileExtensionIdx, FileExtensionSize);

    FileBuf->AllocIdx += RequiredSize;
    FileBuf->FileCount++;
}

static HANDLE mas_internal_directory_win32_find_first(masChar* Path)
{
    int32_t PathSize = MAS_TEXT_LEN(Path);
    if(PathSize <= 0)
        return INVALID_HANDLE_VALUE;

    if(Path[PathSize - 1] != _T('\\'))
        Path[PathSize++] = _T('\\');
    Path[PathSize++] = _T('*');

    HANDLE Handle    = FindFirstFileEx(Path, FindExInfoBasic, &Data, FindExSearchMaxSearchOp, NULL, FIND_FIRST_EX_LARGE_FETCH);
    Path[--PathSize] = _T('\0');

    return Handle;
}

static bool mas_internal_directory_ignore_file(const masChar* Name)
{
    static masChar *List[] = { _T("."), _T("..") };
    static int32_t  Count  = ARRAYSIZE(List);
    for(int32_t i = 0; i < Count; ++i)
        if(strcmp(List[i], Name) == 0)
            return true;
    return false;
}

static bool mas_internal_directory_file_compare_and_add(const masChar* FilePath, const masChar* FileName, int32_t FileNameSize, const masChar* Target, int32_t TargetSize)
{
    if(FileNameSize != TargetSize)
        return false;
    for(int32_t i = 0; i < TargetSize; ++i)
        if(FileName[i] != Target[i])
            return false;
    mas_internal_directory_file_add(FilePath, FileName);
    return true;
}

static void mas_internal_directory_process_found_file(const masChar* FilePath, const masChar* FileName, const masChar** TargetFiles, int32_t TargetCount)
{
    int32_t FileNameSize    = MAS_TEXT_LEN(FileName);
    int32_t FileNameExtIdx  = mas_internal_directory_file_extension_index(FileName, FileNameSize);
    int32_t FilenameExtSize = FileNameSize - FileNameExtIdx;

    for(int32_t TargetIdx = 0; TargetIdx < TargetCount; ++TargetIdx)
    {
        const masChar *Target     = TargetFiels[TargetIdx];
        int32_t        TargetSize = MAS_TEXT_LEN(Target);
        
        if(Target[0] == _T('.') )
        {
            if(mas_internal_directory_file_compare_and_add(FilePath, FileName + FileNameExtIdx, FileNameExtSize, Target, TargetSize))
                break;
        }
        else
        {     
            int32_t TargetExtIdx = mas_internal_directory_file_extension_index(Target, TargetSize);
            if(TargetExtIdx == -1)
            {
                if(mas_internal_directory_file_compare_and_add(FilePath, FileName, FileNameSize - FileNameExtSize, Target, TargetSize))
                    break;
            }
            else
            {
                if(mas_internal_directory_file_compare_and_add(FilePath, FileName, FileNameSize, Target, TargetSize))
                    break;
            }
        }
    }
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

    Directory->WorkPathSize = GetModuleFileName(NULL, Directory->WorkPath, MAS_PATH_SIZE);

    return true;
}

void mas_impl_directory_deinit()
{
    if(!Directory)
        return;
    free(Directory);
    Directory = NULL;
}

bool mas_impl_directory_search_for_files(const masChar* DirectoryPath, const masChar** TargetFiles, int32_t TargetCount)
{
    if(!Directory || !DirectoryPath || !TargetFiles || !(*TargetFiles) || TargetCount <= 0)
        return false;

    int32_t PathSize = MAS_TEXT_LEN(DirectoryPath);
    masChar Path[MAS_PATH_SIZE];
    memset(Path, 0, MAS_PATH_SIZE);
    memcpy(Path, DirectoryPath, PathSize);
    
    WIN32_FIND_DATA Data   = { 0 };
    HANDLE          Handle = mas_internal_directory_win32_find_first(Path);
    if(Handle == INVALID_HANDLE_VALUE)
        return false;

    while(1)
    {
        if(!mas_internal_directory_ignore_file(Data.cFileName))
        {
            if(Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                mas_internal_directory_folder_add(Path, Data.cFileName);
            else
                mas_internal_directory_process_found_file(Path, Data.cFileName, TargetFiles, TargetCount);
        }

        if(!FindNextFile(Handle, &Data))
        {
            FindClose(Handle);
            if(!mas_internal_directory_folder_get_next(Path))
                break;
            
            Handle = mas_internal_directory_win32_find_first(Path);
            if(Handle == INVALID_HANDLE_VALUE)
                break;
        }
    }

    if(Directory->FileBuf.FileCount <= 0)
        return false;

    return true;
}

const masChar* mas_impl_directory_find_folder(const masChar* DirectoryPath, const masChar* FolderName)
{

}