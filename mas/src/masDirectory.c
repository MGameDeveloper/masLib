#include "masTypes.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#include <tchar.h>


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
#define MAS_MEM_SIZE_MB(x)                (1024llu * 1024llu * x##llu)
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_FOLDER_BUF_SIZE                MAS_MEM_SIZE_MB(50)
#define MAS_FILE_BUF_SIZE                  MAS_MEM_SIZE_MB(50)
#define MAS_TEXT_LEN(path)                 mas_internal_directory_path_len(path)
#define MAS_PATH_SIZE                      512


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
typedef struct _masFolder
{
    masChar* Path;
    int32_t  PathSize;
} masFolder;

typedef struct _masFolderBuf
{
    uint8_t Folders[MAS_FOLDER_BUF_SIZE];
    int32_t AllocIdx;
    int32_t QueryIdx;
    int32_t FolderCount;
} masFolderBuf;

typedef struct _masFileBuf
{
    uint8_t  Buf[MAS_FILE_BUF_SIZE];
    uint32_t WriteIdx;
    uint32_t ReadIdx;
    int32_t  FileCount;
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
    int32_t len = _tcslen(Path);
    return (LocalPath - Path);
}

static int32_t mas_internal_directory_file_extension_index(const masChar* FileName, int32_t FileNameSize)
{
    for(int32_t i = FileNameSize - 1; i >= 0; --i)
        if(FileName[i] == MAS_TEXT('.'))
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

    uint64_t      RequiredSize = sizeof(masFolder) + (sizeof(masChar) * FinalPathSize);
    masFolderBuf *FolderBuf    = &Directory->FolderBuf;
    if(FolderBuf->AllocIdx + RequiredSize >= MAS_FOLDER_BUF_SIZE)
    {
        printf("[ ERROR ]: MAS_DIRECTORY_ADD_FOLDER -> internal buffer is full\n");
        return;
    }
    
    masFolder* Folder = MAS_PTR_OFFSET(masFolder, FolderBuf->Folders, FolderBuf->AllocIdx);
    Folder->PathSize  = FinalPathSize;
    Folder->Path      = MAS_PTR_OFFSET(masChar, Folder, sizeof(masFolder));
    memcpy(Folder->Path,            Path,       sizeof(masChar) * Folder->PathSize);
    memcpy(Folder->Path + PathSize, Name,       sizeof(masChar) * NameSize);

    FolderBuf->AllocIdx += RequiredSize;
    FolderBuf->FolderCount++;
}

static const bool mas_internal_directory_folder_get_next(masChar* Path, int32_t *PathSize)
{
    masFolderBuf* FolderBuf = &Directory->FolderBuf;
    if(FolderBuf->QueryIdx >= FolderBuf->AllocIdx)
        return false;
    
    masFolder* Folder = MAS_PTR_OFFSET(masFolder, FolderBuf->Folders, FolderBuf->QueryIdx);
    FolderBuf->QueryIdx += sizeof(masFolder) + (sizeof(masChar) * Folder->PathSize);
    
    memset(Path, 0, sizeof(masChar) * MAS_PATH_SIZE);
    memcpy(Path, Folder->Path, sizeof(masChar) * Folder->PathSize);
    *PathSize = Folder->PathSize;
     
    return true;
}

static masFile* mas_internal_file_add(const masChar* FilePath, const masChar* FileName)
{
    static masChar Temp[MAS_PATH_SIZE] = { 0 };
    memset(Temp, 0, sizeof(masChar) * MAS_PATH_SIZE);

    // Write Path
    masChar* WritePtr = Temp;
    while(*FilePath)
        *WritePtr++ = *FilePath++;

    // Write File Name For Full Path
    masChar* NamePtr = WritePtr;
    while(*FileName)
        *WritePtr++ = *FileName++;
    WritePtr++; // NULL TERMINATOR OF FULL PATH
    uint32_t PathSize = (WritePtr - Temp);
    
    // Write Name Part
    masChar* SubWritePtr = NamePtr;
    while(*SubWritePtr != MAS_TEXT('.'))
        *WritePtr++ = *SubWritePtr++;
    WritePtr++; // NULL TERMINATOR OF FULL PATH
    uint32_t NameSize = (SubWritePtr - NamePtr);

    // Write Extension Part
    masChar* ExtensionPtr = SubWritePtr;
    while(*SubWritePtr)
        *WritePtr++ = *SubWritePtr++;
    WritePtr++; // NULL TERMINATOR OF FULL PATH
    uint32_t ExtensionSize = (SubWritePtr - ExtensionPtr);

    int32_t     CompleteSize = (WritePtr - Temp) * sizeof(masChar);
    int32_t     RequiredSize = sizeof(masFile) + CompleteSize;
    masFileBuf *FindBuf      = &Directory->FileBuf;
    if(FindBuf->WriteIdx + RequiredSize >= MAS_FILE_BUF_SIZE)
        return NULL;

    masFile *File = MAS_PTR_OFFSET(masFile, FindBuf->Buf, FindBuf->WriteIdx);
    File->PathSize      = PathSize;
    File->NameSize      = NameSize;
    File->ExtensionSize = ExtensionSize;
    masChar* Data       = MAS_PTR_OFFSET(masChar, File, sizeof(masFile));
    memcpy(Data, Temp, CompleteSize);
    File->Path          = Data;
    File->Name          = MAS_PTR_OFFSET(masChar, File->Path, sizeof(masChar) * PathSize);
    File->Extension     = MAS_PTR_OFFSET(masChar, File->Name, sizeof(masChar) * (NameSize + 1));

    FindBuf->WriteIdx += RequiredSize;
    FindBuf->FileCount++;

    return File;
}

static const masFile* mas_internal_file_get_next(masFileBuf* FileBuf)
{
    if(!Directory || !FileBuf)
        return NULL;
    if(FileBuf->ReadIdx >= FileBuf->WriteIdx)
        return NULL;
    
    masFile* File = MAS_PTR_OFFSET(masFile, FileBuf->Buf, FileBuf->ReadIdx);  
    FileBuf->ReadIdx += sizeof(masFile) + (sizeof(masChar) * (File->PathSize + File->NameSize + File->ExtensionSize + 2)); // 2 null terminator after name, ext

    return File;
}

static HANDLE mas_internal_directory_win32_find_first(masChar* Path, WIN32_FIND_DATA *Data)
{
    int32_t PathSize = MAS_TEXT_LEN(Path);
    if(PathSize <= 0)
        return INVALID_HANDLE_VALUE;

    if(Path[PathSize - 1] != MAS_TEXT('\\'))
        Path[PathSize++] = MAS_TEXT('\\');
    Path[PathSize++] = MAS_TEXT('*');

    //HANDLE Handle    = FindFirstFileEx(Path, FindExInfoBasic, Data, FindExSearchMaxSearchOp, NULL, FIND_FIRST_EX_LARGE_FETCH);
    memset(Data, 0, sizeof(WIN32_FIND_DATA));
    HANDLE Handle    = FindFirstFile(Path, Data);
    Path[--PathSize] = MAS_TEXT('\0');

    return Handle;
}

static bool mas_internal_directory_ignore_file(const masChar* Name)
{
    static masChar *List[] = { MAS_TEXT("."), MAS_TEXT("..") };
    static int32_t  Count  = ARRAYSIZE(List);
    for(int32_t i = 0; i < Count; ++i)
        if(_tcscmp(List[i], Name) == 0)
            return true;
    return false;
}

static bool mas_internal_directory_file_compare(const masChar* FilePath, const masChar* FileName, int32_t FileNameSize, const masChar* Target, int32_t TargetSize)
{
    if(FileNameSize != TargetSize)
        return false;
    for(int32_t i = 0; i < TargetSize; ++i)
        if(FileName[i] != Target[i])
            return false;

    return true;
}

static masFile* mas_internal_directory_process_found_file(const masChar* FilePath, const masChar* FileName, const masChar** TargetFiles, int32_t TargetCount)
{
    int32_t FileNameSize    = MAS_TEXT_LEN(FileName);
    int32_t FileNameExtIdx  = mas_internal_directory_file_extension_index(FileName, FileNameSize);
    int32_t FileNameExtSize = FileNameSize - FileNameExtIdx;

    masFile *File = NULL;
    for(int32_t TargetIdx = 0; TargetIdx < TargetCount; ++TargetIdx)
    {
        const masChar *Target     = TargetFiles[TargetIdx];
        int32_t        TargetSize = MAS_TEXT_LEN(Target);
        
        if(Target[0] == MAS_TEXT('.') )
        {       
            if(mas_internal_directory_file_compare(FilePath, FileName + FileNameExtIdx, FileNameExtSize, Target, TargetSize))
            {
                File = mas_internal_file_add(FilePath, FileName);
                break;
            }    
        }
        else
        {     
            int32_t TargetExtIdx = mas_internal_directory_file_extension_index(Target, TargetSize);
            if(TargetExtIdx == -1)
            {
                if(mas_internal_directory_file_compare(FilePath, FileName, FileNameSize - FileNameExtSize, Target, TargetSize))
                {
                    File = mas_internal_file_add(FilePath, FileName);
                    break;
                } 
            }
            else
            {
                if(mas_internal_directory_file_compare(FilePath, FileName, FileNameSize, Target, TargetSize))
                {
                    File = mas_internal_file_add(FilePath, FileName);
                    break;
                } 
            }
        }
    }

    return File;
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
    if(Directory->WorkPathSize <= 0)
    {
        free(Directory);
        Directory = NULL;
        return false;
    }

    PathRemoveFileSpec(Directory->WorkPath);
    Directory->WorkPathSize = MAS_TEXT_LEN(Directory->WorkPath);
    
    return true;
}

void mas_impl_directory_deinit()
{
    if(!Directory)
        return;
    free(Directory);
    Directory = NULL;
}

int32_t mas_impl_directory_current_path(masChar* Path, int32_t PathSize)
{
    if(!Directory)
        return 0;
    if(Directory->WorkPathSize > PathSize)
        return 0;
    memset(Path, 0, sizeof(masChar) * PathSize);
    memcpy(Path, Directory->WorkPath, sizeof(masChar) * Directory->WorkPathSize);
    return Directory->WorkPathSize;
}

masFileBuf* mas_impl_directory_search_for_files(const masChar* DirectoryPath, const masChar** TargetFiles, int32_t TargetCount)
{
    if(!Directory || !DirectoryPath || !TargetFiles || !(*TargetFiles) || TargetCount <= 0)
        return NULL;

    int32_t PathSize = MAS_TEXT_LEN(DirectoryPath);
    masChar Path[MAS_PATH_SIZE];
    memset(Path, 0, sizeof(masChar) * MAS_PATH_SIZE);
    memcpy(Path, DirectoryPath, sizeof(masChar) * PathSize);
    
    WIN32_FIND_DATA Data   = { 0 };
    HANDLE          Handle = mas_internal_directory_win32_find_first(Path, &Data);
    if(Handle == INVALID_HANDLE_VALUE)
        return NULL;

    while(1)
    {
        if(!mas_internal_directory_ignore_file(Data.cFileName))
        {
            if(Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                mas_internal_directory_folder_add(Path, Data.cFileName);
            else
            {
                masFile* File = mas_internal_directory_process_found_file(Path, Data.cFileName, TargetFiles, TargetCount);
                if(File)
                    File->Size = ((uint64_t)Data.nFileSizeHigh << 32) | Data.nFileSizeLow;
            }   
        }

        if(!FindNextFile(Handle, &Data))
        {
            FindClose(Handle);
            if(!mas_internal_directory_folder_get_next(Path, &PathSize))
                break;
            
            Handle = mas_internal_directory_win32_find_first(Path, &Data);
            if(Handle == INVALID_HANDLE_VALUE)
                break;
        }
    }

    if(Directory->FileBuf.FileCount <= 0)
        return NULL;

    return &Directory->FileBuf;
}

const masFile* mas_impl_directory_filebuf_next_file(masFileBuf* FileBuf)
{
    return mas_internal_file_get_next(FileBuf);
}

int32_t mas_impl_directory_find_folder(const masChar* DirectoryPath, const masChar* FolderName, masChar* FolderPath, int32_t FolderPathSize)
{
    if(!Directory || !DirectoryPath || !FolderName || !FolderPath)
        return false;

    int32_t PathSize = MAS_TEXT_LEN(DirectoryPath);
    masChar Path[MAS_PATH_SIZE];
    memset(Path, 0, sizeof(masChar) * MAS_PATH_SIZE);
    memcpy(Path, DirectoryPath, sizeof(masChar) * PathSize);
    
    WIN32_FIND_DATA Data   = { 0 };
    HANDLE          Handle = mas_internal_directory_win32_find_first(Path, &Data);
    if(Handle == INVALID_HANDLE_VALUE)
        return 0;

    while(1)
    {
        if(!mas_internal_directory_ignore_file(Data.cFileName))
        {
            if(Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(_tcscmp(FolderName, Data.cFileName) != 0)
                    mas_internal_directory_folder_add(Path, Data.cFileName);
                else
                {
                    int32_t FolderNameSize = MAS_TEXT_LEN(Data.cFileName);
                    int32_t CompletePathSize = PathSize + FolderNameSize + 1; // + 1 null terminator
                    if(FolderPathSize < CompletePathSize)
                        return 0;
                    memset(FolderPath, 0, FolderPathSize);
                    memcpy(FolderPath,                               Path,           sizeof(masChar) * PathSize);
                    memcpy(FolderPath + PathSize,                    Data.cFileName, sizeof(masChar) * FolderNameSize);
                    memcpy(FolderPath + (PathSize + FolderNameSize), MAS_TEXT("\\"),     sizeof(masChar));

                    FindClose(Handle);
                    return CompletePathSize;
                }
            }
        }

        if(!FindNextFile(Handle, &Data))
        {
            FindClose(Handle);
            if(!mas_internal_directory_folder_get_next(Path, &PathSize))
                break;
            
            Handle = mas_internal_directory_win32_find_first(Path, &Data);
            if(Handle == INVALID_HANDLE_VALUE)
                break;
        }
    }

    return 0;
}









