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
#define MAS_FILE_BUF_SIZE                  16384                 
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

typedef struct _masFileGroup
{
    uint8_t  *Buf;
    uint64_t  BufSize;
    uint32_t  AddIdx;
    uint32_t  GetIdx;
    int32_t   ResizeFactor;
    int32_t   ResizeCounter;
    int32_t   FileCounter;
} masFileGroup;

typedef struct _masDirecotry
{
    masFolderBuf FolderBuf;
    //masFileBuf   FileBuf;
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

static bool mas_internal_directory_folder_get_next(masChar* Path, int32_t *PathSize)
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

static masFile* mas_internal_directory_file_group_add(masFileGroup** FileGroup, uint64_t PathSize)
{
    if(!FileGroup || PathSize < 0)
        return NULL;

    uint64_t      FileSize = sizeof(masFile) + (sizeof(masChar) * PathSize);
    masFileGroup *Group    = *FileGroup;
    if(!Group)
    {
        uint64_t BufSize = sizeof(masChar) * MAS_FILE_BUF_SIZE;
        uint64_t MemSize = sizeof(masFileGroup) + BufSize;
        Group = (masFileGroup*)malloc(MemSize);
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
            uint64_t BufSize = sizeof(masChar) * MAS_FILE_BUF_SIZE * Group->ResizeFactor;
            uint64_t MemSize = sizeof(masFileGroup) + BufSize;
            masFileGroup* NewGroup = realloc(Group, MemSize);
            if(!NewGroup)
                return NULL;
            Group                = NewGroup;
            Group->Buf           = MAS_PTR_OFFSET(uint8_t, Group, sizeof(masFileGroup));
            Group->BufSize       = BufSize;
            Group->ResizeFactor *= 2;
            Group->ResizeCounter++;
        }
    }

    masFile* File  = MAS_PTR_OFFSET(masFile, Group->Buf, Group->AddIdx);
    File->Path     = MAS_PTR_OFFSET(masChar, File, sizeof(masFile));
    File->PathSize = PathSize;

    Group->FileCounter++;
    Group->AddIdx += FileSize;

    *FileGroup = Group;

    return File;
}

static bool mas_internal_directory_compare_names(const masChar* TargetName, int32_t TargetNameSize, const masChar* Name, int32_t NameSize)
{
    if(!TargetName || !Name || NameSize <= 0 || TargetNameSize <= 0)
        return false;

    if(TargetName[0] == MAS_TEXT('.'))
    {
        if(Name[NameSize - 1] == MAS_TEXT('h') && Name[NameSize - 2] == MAS_TEXT('.'))
        {
            int32_t w = 0;
        }

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
            if(TargetName[i] == MAS_TEXT('.'))
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


bool mas_impl_directory_find_file(const masChar* TargetPath, const masChar* FileName, masChar* OutBuf, int32_t BufSize)
{
    if(!TargetPath || !FileName)
        return false;

    int32_t TargetNameSize = MAS_TEXT_LEN(FileName);
    int32_t TargetPathSize = MAS_TEXT_LEN(TargetPath);
    if(TargetPathSize <= 0 || TargetNameSize <= 0)
        return false;

    static masChar Path[MAS_PATH_SIZE];
    memset(Path, 0, sizeof(masChar) * MAS_PATH_SIZE);
    memcpy(Path, TargetPath, sizeof(masChar) * TargetPathSize);

    WIN32_FIND_DATA Data;
    memset(&Data, 0, sizeof(Data));

    HANDLE Handle = mas_internal_directory_win32_find_first(Path, &Data);
    if(Handle == INVALID_HANDLE_VALUE)
        return false;

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

                    memset(OutBuf,                  0,              sizeof(masChar) * BufSize);
                    memcpy(OutBuf,                  Path,           sizeof(masChar) * TargetPathSize);
                    memcpy(OutBuf + TargetPathSize, Data.cFileName, sizeof(masChar) * NameSize);
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

    if(OutBuf[0] == MAS_TEXT('\0'))
        return false;

    return true;
}

const masFileGroup* mas_impl_directory_find_file_group(const masChar* TargetPath, const masChar* TargetName)
{
        if(!TargetPath || !TargetName)
        return false;

    int32_t TargetNameSize = MAS_TEXT_LEN(TargetName);
    int32_t TargetPathSize = MAS_TEXT_LEN(TargetPath);
    if(TargetPathSize <= 0 || TargetNameSize <= 0)
        return false;
    static masChar Path[MAS_PATH_SIZE];
    memset(Path, 0, sizeof(masChar) * MAS_PATH_SIZE);
    memcpy(Path, TargetPath, sizeof(masChar) * TargetPathSize);

    WIN32_FIND_DATA Data;
    memset(&Data, 0, sizeof(Data));

    HANDLE Handle = mas_internal_directory_win32_find_first(Path, &Data);
    if(Handle == INVALID_HANDLE_VALUE)
        return NULL;

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
                    uint64_t PathSize = sizeof(masChar) * (TargetPathSize + NameSize + 1);
                    masFile* File = mas_internal_directory_file_group_add(&FileGroup, PathSize);
                    if(File)
                    {
                        memcpy(File->Path,                  Path,           sizeof(masChar) * TargetPathSize);
                        memcpy(File->Path + TargetPathSize, Data.cFileName, sizeof(masChar) * NameSize);
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

const masFile* mas_impl_file_group_next_file(const masFileGroup* FileGroup)
{
    if(!Directory || !FileGroup)
        return NULL;
    
    masFileGroup* Group = (masFileGroup*)FileGroup;
    if(Group->GetIdx >= Group->AddIdx)
        return NULL;

    masFile* File = MAS_PTR_OFFSET(masFile, Group->Buf, Group->GetIdx);
    Group->GetIdx += sizeof(masFile) + (sizeof(masChar) * File->PathSize);

    return File;
}