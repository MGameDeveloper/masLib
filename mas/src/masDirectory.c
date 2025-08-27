#include "masImpl.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#include <tchar.h>


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_FOLDER_BUF_SIZE                4096
#define MAS_FILE_BUF_SIZE                  16384                 
#define MAS_PATH_SIZE                      512
#define MAS_TEXT_LEN(path)                 mas_internal_directory_path_len(path)


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
typedef struct _masFolder
{
    uint32_t PathSize;
} masFolder;

typedef struct _masFolderBuf
{
    uint8_t  *Buf;
    uint64_t  BufSize;
    uint32_t  AddIdx;
    uint32_t  GetIdx;
    int32_t   ResizeFactor;
    int32_t   ResizeCounter;
    int32_t   FolderCounter;
} masFolderBuf;

struct _masFileGroup
{
    uint8_t  *Buf;
    uint64_t  BufSize;
    uint32_t  AddIdx;
    uint32_t  GetIdx;
    int32_t   ResizeFactor;
    int32_t   ResizeCounter;
    int32_t   FileCounter;
};

typedef struct _masDirecotry
{
    masFolderBuf *FolderBuf;
    int32_t       WorkPathSize;
    char       WorkPath[MAS_PATH_SIZE];
} masDirectory;


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
static masDirectory Directory = { 0 };


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
static int32_t mas_internal_directory_path_len(const char* Path)
{
    const char* LocalPath = Path;
    while(*LocalPath) 
        ++LocalPath;
    return (LocalPath - Path);
}

static void mas_internal_directory_folder_add(const char* Path, const char* Name)
{
    if(!Path || !Name)
        return;
    
    int32_t       CopyPathSize = MAS_TEXT_LEN(Path);
    int32_t       CopyNameSize = MAS_TEXT_LEN(Name);
    uint64_t      PathSize     = CopyPathSize + CopyNameSize + 2; // + 2, 1 for \\ and 1 for null terminator
    uint64_t      FolderSize   = sizeof(masFolder) + (sizeof(char) * PathSize);
    masFolderBuf *FolderBuf    = Directory.FolderBuf;
    if(!FolderBuf)
    {
        uint64_t BufSize = sizeof(char) * MAS_FOLDER_BUF_SIZE;
        uint64_t MemSize = sizeof(masFolderBuf) + BufSize;
        FolderBuf = MAS_IMPL_MALLOC(masFolderBuf, MemSize);
        if(!FolderBuf)
            return;
        memset(FolderBuf, 0, MemSize);
        FolderBuf->Buf           = MAS_PTR_OFFSET(uint8_t, FolderBuf, sizeof(masFolderBuf));
        FolderBuf->BufSize       = BufSize;
        FolderBuf->ResizeFactor  = 2;
        FolderBuf->AddIdx        = 0;
        FolderBuf->ResizeCounter = 0;
        FolderBuf->GetIdx        = 0;
        FolderBuf->FolderCounter = 0;
    }
    else
    {
        if(FolderBuf->AddIdx + FolderSize >= FolderBuf->BufSize)
        {
            uint64_t BufSize = sizeof(char) * MAS_FOLDER_BUF_SIZE * FolderBuf->ResizeFactor;
            uint64_t MemSize = sizeof(masFolderBuf) + BufSize;
            masFolderBuf* NewFolderBuf = MAS_IMPL_REALLOC(masFolderBuf, FolderBuf, MemSize);
            if(!NewFolderBuf)
                return;
            FolderBuf                = NewFolderBuf;
            FolderBuf->Buf           = MAS_PTR_OFFSET(uint8_t, FolderBuf, sizeof(masFolderBuf));
            FolderBuf->BufSize       = BufSize;
            FolderBuf->ResizeFactor *= 2;
            FolderBuf->ResizeCounter++;
        }
    }

    masFolder *Folder     = MAS_PTR_OFFSET(masFolder, FolderBuf->Buf, FolderBuf->AddIdx);
    char      *FolderPath = MAS_PTR_OFFSET(char, Folder, sizeof(masFolder));
    Folder->PathSize      = PathSize;
    memcpy(FolderPath,                Path, sizeof(char) * CopyPathSize);
    memcpy(FolderPath + CopyPathSize, Name, sizeof(char) * CopyNameSize);
    FolderPath[Folder->PathSize - 2] = '\\';

    FolderBuf->FolderCounter++;
    FolderBuf->AddIdx += FolderSize;

    Directory.FolderBuf = FolderBuf;
}

static bool mas_internal_directory_folder_get_next(char* Path, int32_t *PathSize)
{
    masFolderBuf* FolderBuf = Directory.FolderBuf;
    if(FolderBuf->GetIdx >= FolderBuf->AddIdx)
        return false;
    
    masFolder  *Folder     = MAS_PTR_OFFSET(masFolder, FolderBuf->Buf, FolderBuf->GetIdx);
    const char *FolderPath = MAS_PTR_OFFSET(const char, Folder, sizeof(masFolder));
    FolderBuf->GetIdx     += sizeof(masFolder) + (sizeof(char) * Folder->PathSize);
    
    *PathSize = Folder->PathSize - 1;
    memset(Path, 0,          sizeof(char) * MAS_PATH_SIZE);
    memcpy(Path, FolderPath, sizeof(char) * (*PathSize));
     
    return true;
}

static void mas_internal_directory_folder_reset()
{
    if(!Directory.FolderBuf)
        return;
    masFolderBuf* FolderBuf = Directory.FolderBuf;
    memset(FolderBuf->Buf, 0, FolderBuf->BufSize);
    FolderBuf->AddIdx        = 0;
    FolderBuf->GetIdx        = 0;
    FolderBuf->FolderCounter = 0;
}

static HANDLE mas_internal_directory_win32_find_first(char* Path, WIN32_FIND_DATA *Data)
{
    int32_t PathSize = MAS_TEXT_LEN(Path);
    if(PathSize <= 0)
        return INVALID_HANDLE_VALUE;

    if(Path[PathSize - 1] != '\\')
        Path[PathSize++] = '\\';
    Path[PathSize++] = '*';

    //HANDLE Handle    = FindFirstFileEx(Path, FindExInfoBasic, Data, FindExSearchMaxSearchOp, NULL, FIND_FIRST_EX_LARGE_FETCH);
    memset(Data, 0, sizeof(WIN32_FIND_DATA));
    HANDLE Handle    = FindFirstFile(Path, Data);
    Path[--PathSize] = '\0';

    return Handle;
}

static bool mas_internal_directory_ignore_file(const char* Name)
{
    static char *List[] = { ".", ".." };
    static int32_t  Count  = ARRAYSIZE(List);
    for(int32_t i = 0; i < Count; ++i)
        if(_tcscmp(List[i], Name) == 0)
            return true;
    return false;
}

static char* mas_internal_directory_file_group_add(masFileGroup** FileGroup, uint64_t PathSize)
{
    if(!FileGroup || PathSize < 0)
        return NULL;

    uint64_t      FileSize = sizeof(masFile) + (sizeof(char) * PathSize);
    masFileGroup *Group    = *FileGroup;
    if(!Group)
    {
        uint64_t BufSize = sizeof(char) * MAS_FILE_BUF_SIZE;
        uint64_t MemSize = sizeof(masFileGroup) + BufSize;
        Group = MAS_IMPL_MALLOC(masFileGroup, MemSize);
        if(!Group)
            return NULL;
        memset(Group, 0, MemSize);
        Group->Buf           = MAS_PTR_OFFSET(uint8_t, Group, sizeof(masFileGroup));
        Group->BufSize       = BufSize;
        Group->ResizeFactor  = 2;
        Group->AddIdx        = 0;
        Group->ResizeCounter = 0;
        Group->GetIdx        = 0;
        Group->FileCounter   = 0;
    }
    else
    {
        if(Group->AddIdx + FileSize >= Group->BufSize)
        {
            uint64_t      BufSize  = sizeof(char) * MAS_FILE_BUF_SIZE * Group->ResizeFactor;
            uint64_t      MemSize  = sizeof(masFileGroup) + BufSize;
            masFileGroup *NewGroup = MAS_IMPL_REALLOC(masFileGroup, Group, MemSize);
            if(!NewGroup)
                return NULL;
            Group                = NewGroup;
            Group->Buf           = MAS_PTR_OFFSET(uint8_t, Group, sizeof(masFileGroup));
            Group->BufSize       = BufSize;
            Group->ResizeFactor *= 2;
            Group->ResizeCounter++;
        }
    }

    masFile *File  = MAS_PTR_OFFSET(masFile, Group->Buf, Group->AddIdx);
    char    *Path  = MAS_PTR_OFFSET(char, File, sizeof(masFile));
    File->PathSize = PathSize;

    Group->FileCounter++;
    Group->AddIdx += FileSize;

    *FileGroup = Group;
    
    return Path;
}

static bool mas_internal_directory_compare_names(const char* TargetName, int32_t TargetNameSize, const char* Name, int32_t NameSize)
{
    if(!TargetName || !Name || NameSize <= 0 || TargetNameSize <= 0)
        return false;

    if(TargetName[0] == '.')
    {
        bool SameExt = true;
        int32_t i = TargetNameSize - 1;
        int32_t j = NameSize - 1;
        for(; (i >= 0 && j >= 0) ; --i, --j)
        {
            if(TargetName[i] != Name[j])
            {
                SameExt = false;
                break;
            }
        }  

        if(SameExt)
            return true;
    }
    else
    {
        bool TargetHasExt = false;
        for(int32_t i = TargetNameSize - 1; i >= 0; --i)
        {
            if(TargetName[i] == '.')
            {
                TargetHasExt = true;
                break;
            }
        }
    
        if(TargetHasExt && (NameSize == TargetNameSize))
        {
            if(_tcscmp(TargetName, Name) == 0)
                return true;
        }
        else if(!TargetHasExt && TargetNameSize < NameSize)
        {
            bool SameName = true;
            for(int32_t i = 0; i < TargetNameSize; ++i)
            {
                if(TargetName[i] != Name[i])
                {
                    SameName = false;
                    break;
                }
            }

            if(SameName)
                return true;
        }
    }

    return false;
}


/***************************************************************************************************************************
* 
****************************************************************************************************************************/
bool mas_impl_directory_init()
{
    if(Directory.WorkPathSize > 0)
        return true;

    Directory.WorkPathSize = GetModuleFileName(NULL, Directory.WorkPath, MAS_PATH_SIZE);
    if(Directory.WorkPathSize <= 0)
        return false;

    PathRemoveFileSpec(Directory.WorkPath);
    Directory.WorkPathSize = MAS_TEXT_LEN(Directory.WorkPath);
    
    return true;
}

void mas_impl_directory_deinit()
{
    if(!Directory.FolderBuf)
        return;
    MAS_IMPL_FREE(Directory.FolderBuf);
}

int32_t mas_impl_directory_current_path(char* Path, int32_t PathSize)
{
    if(Directory.WorkPathSize > PathSize)
        return 0;
    memset(Path, 0, sizeof(char) * PathSize);
    memcpy(Path, Directory.WorkPath, sizeof(char) * Directory.WorkPathSize);
    return Directory.WorkPathSize;
}


bool mas_impl_directory_find_file(const char* TargetPath, const char* FileName, char* OutBuf, int32_t BufSize)
{
    if(!TargetPath || !FileName)
        return false;

    int32_t TargetNameSize = MAS_TEXT_LEN(FileName);
    int32_t TargetPathSize = MAS_TEXT_LEN(TargetPath);
    if(TargetPathSize <= 0 || TargetNameSize <= 0)
        return false;

    static char Path[MAS_PATH_SIZE];
    memset(Path, 0, sizeof(char) * MAS_PATH_SIZE);
    memcpy(Path, TargetPath, sizeof(char) * TargetPathSize);

    WIN32_FIND_DATA Data;
    memset(&Data, 0, sizeof(Data));

    HANDLE Handle = mas_internal_directory_win32_find_first(Path, &Data);
    if(Handle == INVALID_HANDLE_VALUE)
        return false;

    mas_internal_directory_folder_reset();

    while(1)
    {
        if(!mas_internal_directory_ignore_file(Data.cFileName))
        {
            if(Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                mas_internal_directory_folder_add(Path, Data.cFileName);
            else
            {
                int32_t NameSize = MAS_TEXT_LEN(Data.cFileName);
                if(mas_internal_directory_compare_names(FileName, TargetNameSize, Data.cFileName, NameSize))
                {
                    uint64_t PathSize = TargetPathSize + NameSize;
                    if(BufSize < PathSize)
                        break;

                    memset(OutBuf,                  0,              sizeof(char) * BufSize);
                    memcpy(OutBuf,                  Path,           sizeof(char) * TargetPathSize);
                    memcpy(OutBuf + TargetPathSize, Data.cFileName, sizeof(char) * NameSize);
                    break;
                }
            }
        }

        if(!FindNextFile(Handle, &Data))
        {
            FindClose(Handle);
            Handle = INVALID_HANDLE_VALUE;

            if(!mas_internal_directory_folder_get_next(Path, &TargetPathSize))
                break;
            Handle = mas_internal_directory_win32_find_first(Path, &Data);
            if(Handle == INVALID_HANDLE_VALUE)
                break;
        }
    }
    
    if(Handle != INVALID_HANDLE_VALUE)
        FindClose(Handle);

    if(OutBuf[0] == '\0')
        return false;

    return true;
}

masFileGroup* mas_impl_directory_find_files(const char* TargetPath, const char* TargetName)
{
    if(!TargetPath || !TargetName)
        return false;

    int32_t TargetNameSize = MAS_TEXT_LEN(TargetName);
    int32_t TargetPathSize = MAS_TEXT_LEN(TargetPath);
    if(TargetPathSize <= 0 || TargetNameSize <= 0)
        return false;
    static char Path[MAS_PATH_SIZE];
    memset(Path, 0, sizeof(char) * MAS_PATH_SIZE);
    memcpy(Path, TargetPath, sizeof(char) * TargetPathSize);

    WIN32_FIND_DATA Data;
    memset(&Data, 0, sizeof(Data));

    HANDLE Handle = mas_internal_directory_win32_find_first(Path, &Data);
    if(Handle == INVALID_HANDLE_VALUE)
        return NULL;

    mas_internal_directory_folder_reset();

    masFileGroup* FileGroup = NULL;
    while(1)
    {
        if(!mas_internal_directory_ignore_file(Data.cFileName))
        {
            if(Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                mas_internal_directory_folder_add(Path, Data.cFileName);
            else
            {
                int32_t NameSize = MAS_TEXT_LEN(Data.cFileName);
                if(mas_internal_directory_compare_names(TargetName, TargetNameSize, Data.cFileName, NameSize))
                {
                    uint64_t PathSize = TargetPathSize + NameSize + 1;
                    char* FilePath = mas_internal_directory_file_group_add(&FileGroup, PathSize);
                    if(FilePath)
                    {
                        memcpy(FilePath,                  Path,           sizeof(char) * TargetPathSize);
                        memcpy(FilePath + TargetPathSize, Data.cFileName, sizeof(char) * NameSize);
                    }
                }
            }
        }

        if(!FindNextFile(Handle, &Data))
        {
            FindClose(Handle);
            Handle = INVALID_HANDLE_VALUE;

            if(!mas_internal_directory_folder_get_next(Path, &TargetPathSize))
                break;
            Handle = mas_internal_directory_win32_find_first(Path, &Data);
            if(Handle == INVALID_HANDLE_VALUE)
                break;
        }
    }

    if(Handle != INVALID_HANDLE_VALUE)
        FindClose(Handle);
    
    return FileGroup;
}

masFileGroup* mas_impl_directory_find_mix_files(const char* TargetPath, const char** TargetList, int32_t TargetCount)
{
    if(!TargetPath || !TargetList)
        return false;

    int32_t TargetPathSize = MAS_TEXT_LEN(TargetPath);
    if(TargetPathSize <= 0)
        return false;
    static char Path[MAS_PATH_SIZE];
    memset(Path, 0, sizeof(char) * MAS_PATH_SIZE);
    memcpy(Path, TargetPath, sizeof(char) * TargetPathSize);

    WIN32_FIND_DATA Data;
    memset(&Data, 0, sizeof(Data));

    HANDLE Handle = mas_internal_directory_win32_find_first(Path, &Data);
    if(Handle == INVALID_HANDLE_VALUE)
        return NULL;

    mas_internal_directory_folder_reset();

    masFileGroup* FileGroup = NULL;
    while(1)
    {
        if(!mas_internal_directory_ignore_file(Data.cFileName))
        {
            if(Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                mas_internal_directory_folder_add(Path, Data.cFileName);
            else
            {
                int32_t NameSize = MAS_TEXT_LEN(Data.cFileName);
                for(int32_t i = 0; i < TargetCount; ++i)
                {
                    const char *TargetName     = TargetList[i];
                    int32_t        TargetNameSize = MAS_TEXT_LEN(TargetName);
                    if(mas_internal_directory_compare_names(TargetName, TargetNameSize, Data.cFileName, NameSize))
                    {
                        uint64_t PathSize = TargetPathSize + NameSize + 1;
                        char* FilePath = mas_internal_directory_file_group_add(&FileGroup, PathSize);
                        if(FilePath)
                        {
                            memcpy(FilePath,                  Path,           sizeof(char) * TargetPathSize);
                            memcpy(FilePath + TargetPathSize, Data.cFileName, sizeof(char) * NameSize);
                        }
                        break;
                    }
                }    
            }
        }

        if(!FindNextFile(Handle, &Data))
        {
            FindClose(Handle);
            Handle = INVALID_HANDLE_VALUE;

            if(!mas_internal_directory_folder_get_next(Path, &TargetPathSize))
                break;
            Handle = mas_internal_directory_win32_find_first(Path, &Data);
            if(Handle == INVALID_HANDLE_VALUE)
                break;
        }
    }

    if(Handle != INVALID_HANDLE_VALUE)
        FindClose(Handle);
    
    return FileGroup;
}

const masFile* mas_impl_directory_file_group_next_file(masFileGroup* FileGroup)
{
    if(!FileGroup)
        return NULL;
    
    if(FileGroup->GetIdx >= FileGroup->AddIdx)
        return NULL;

    masFile* File = MAS_PTR_OFFSET(masFile, FileGroup->Buf, FileGroup->GetIdx);
    FileGroup->GetIdx += sizeof(masFile) + (sizeof(char) * File->PathSize);

    return File;
}

const char* mas_impl_directory_file_path(const masFile* File)
{
    if(!File || File->PathSize <= 0)
        return NULL;

    const char* Path = MAS_PTR_OFFSET(char, File, sizeof(masFile));
    return Path;
}

uint32_t mas_impl_directory_file_group_file_count(masFileGroup* FileGroup)
{
    if(!FileGroup)
        return 0;
    return FileGroup->FileCounter;
}

void mas_impl_directory_file_group_destroy(masFileGroup** FileGroup)
{
    if(!FileGroup || !(*FileGroup))
        return;

    //free(*FileGroup);
    //*FileGroup = NULL;
    MAS_IMPL_FREE(FileGroup);
}