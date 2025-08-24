
#include "masImpl.h"


/*********************************************************************************************************
*
**********************************************************************************************************/
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_INIT_CAPACITY (1024u * 8u)


/*********************************************************************************************************
*
**********************************************************************************************************/
typedef enum _masTrackType
{
    TRACK_TYPE_ALLOCATE,
    TRACK_TYPE_RESIZE,
    TRACK_TYPE_FREE,

    TRACK_TYPE_COUNT
} masTrackType;

typedef struct _masAlloc
{
    const void    *Data;
    const masChar *File[TRACK_TYPE_COUNT];
    const masChar *Source;
    uint64_t       Size;
    uint32_t       Line[TRACK_TYPE_COUNT];
} masAlloc;

typedef struct _masMemory
{
    masAlloc *Allocs;
    int32_t   Capacity;
    int32_t   AllocCount;
    int32_t   ResizeFactor;
    int32_t   ResizeCounter;
    uint32_t  LargestAllocSize;
    uint32_t  TotalSize;
    uint32_t  UsedSize;
} masMemory;


/*********************************************************************************************************
*
**********************************************************************************************************/
static masMemory Memory = { 0 };


/*********************************************************************************************************
*
**********************************************************************************************************/
static void mas_internal_memory_track_alloc(const void* Mem, uint64_t Size, const masChar* Source, const masChar* File, uint32_t Line)
{
    if(!Memory.Allocs)
    {
        uint64_t MemSize = sizeof(masAlloc) * MAS_INIT_CAPACITY;
        void* Ptr = malloc(MemSize);
        assert(Ptr != NULL && "Tracking memory list allocating failed");

        memset(Ptr, 0, MemSize);  
        Memory.Allocs       = MAS_PTR_OFFSET(masAlloc, Ptr, 0);
        Memory.Capacity     = MAS_INIT_CAPACITY;
        Memory.ResizeFactor = 2;
    }
    else if(Memory.AllocCount + 1 >= Memory.Capacity)
    {
        uint64_t  Capacity = MAS_INIT_CAPACITY * Memory.ResizeFactor;
        uint64_t  MemSize  = sizeof(masAlloc) * Capacity;
        void     *Ptr      = realloc(Memory.Allocs, MemSize);
        assert(Ptr != NULL && "Tracking memory list resizing failed");

        Memory.Allocs        = MAS_PTR_OFFSET(masAlloc, Ptr, 0);
        Memory.Capacity      = Capacity;
        Memory.ResizeFactor *= 2;
        Memory.ResizeCounter++;
    }

    masAlloc *Alloc                  = &Memory.Allocs[Memory.AllocCount++];
    Alloc->Data                      = Mem;
    Alloc->File[TRACK_TYPE_ALLOCATE] = File;
    Alloc->Line[TRACK_TYPE_ALLOCATE] = Line;
    Alloc->Size                      = Size;
    Alloc->Source                    = Source;

    if(Size > Memory.LargestAllocSize)
        Memory.LargestAllocSize = Size;

    Memory.UsedSize += Size;
    if(Memory.UsedSize > Memory.TotalSize)
        Memory.TotalSize = Memory.UsedSize;
}

static void mas_internal_memory_track_resize(const void* OldMem, const void* NewMem, uint64_t Size, const masChar* Source, const masChar* File, uint32_t Line)
{
    if(!Memory.Allocs)
        return;

    masAlloc *Alloc = NULL;
    for(int32_t i = 0; i < Memory.AllocCount; ++i)
    {
        if(Memory.Allocs[i].Data == OldMem)
        {
            Alloc = &Memory.Allocs[i];
            break;
        }
    }

    if(Alloc)
    {
        Alloc->Data                    = NewMem;
        Alloc->File[TRACK_TYPE_RESIZE] = File;
        Alloc->Line[TRACK_TYPE_RESIZE] = Line;
        Alloc->Size                    = Size;
        Alloc->Source                  = Source;

        if(Size > Memory.LargestAllocSize)
            Memory.LargestAllocSize = Size;
    }
}

static void mas_internal_memory_track_free(const void* Mem, const masChar* Source, const masChar* File, uint32_t Line)
{
    if(!Memory.Allocs)
        return;
    
    masAlloc *Alloc = NULL;
    for(int32_t i = 0; i < Memory.AllocCount; ++i)
    {
        if(Memory.Allocs[i].Data == Mem)
        {
            Alloc = &Memory.Allocs[i];
            break;
        }
    }

    if(Alloc)
    {
        Alloc->File[TRACK_TYPE_FREE] = File;
        Alloc->Line[TRACK_TYPE_FREE] = Line;
        Alloc->Source                = Source;

        Memory.UsedSize -= Alloc->Size;
    }
}


/*********************************************************************************************************
*
**********************************************************************************************************/
void* mas_impl_memory_alloc(uint64_t Size, const masChar* Source, const masChar* File, uint32_t Line)
{
    void* Mem = malloc(Size);
    if(!Mem)
        return NULL;
    memset(Mem, 0, Size);
    mas_internal_memory_track_alloc(Mem, Size, Source, File, Line);
    return Mem;
}

void* mas_impl_memory_resize(void* Mem, uint64_t Size, const masChar* Source, const masChar* File, uint32_t Line)
{
    void* LocalMem = realloc(Mem, Size);
    if(!LocalMem)
        return Mem;
    mas_internal_memory_track_resize(Mem, LocalMem, Size, Source, File, Line);
    return LocalMem;
}

void mas_impl_memory_free(void** Mem, const masChar* Source, const masChar* File, uint32_t Line)
{
    if(Mem && *Mem)
    {
        mas_internal_memory_track_free(*Mem, Source, File, Line);
        free(*Mem);
        *Mem = NULL;
    }
}

void mas_impl_memory_copy(void* Dest, const void* Src, uint64_t Size)
{
    memcpy(Dest, Src, Size);
}

void mas_impl_memory_move(void* Dest, const void* Src, uint64_t Size)
{
    memmove(Dest, Src, Size);
}

void mas_impl_memory_zero(void* Mem, uint64_t Size)
{
    memset(Mem, 0, Size);
}

void mas_impl_memory_set(void* Mem, int32_t Value, uint64_t Size)
{
    memset(Mem, Value, Size);
}

void mas_impl_memory_dump()
{
    if(!Memory.Allocs)
        return;
    
    masAlloc* Alloc = NULL;
    for(int32_t i = 0; i < Memory.AllocCount; ++i)
    {
        Alloc = &Memory.Allocs[i];

        mas_impl_log(MAS_TEXT("0x%p:"), Alloc->Data);
        if(Alloc->File[TRACK_TYPE_FREE])
            mas_impl_log(MAS_TEXT(" NULL\n"));
        else
        {
            for(int32_t b = 0; b < Alloc->Size; ++b)
                mas_impl_log(MAS_TEXT(" %1x"), ((const uint8_t*)Alloc->Data)[b]);
            mas_impl_log(MAS_TEXT("\n"));
        }
    }
}

void mas_impl_memory_leak_detect()
{
    if(!Memory.Allocs)
        return;

    uint32_t  LeakCount = 0;
    masAlloc *Alloc     = NULL;
    for(int32_t i = 0; i < Memory.AllocCount; ++i)
    {
        Alloc = &Memory.Allocs[i];
        if(Alloc->File[TRACK_TYPE_ALLOCATE] && !Alloc->File[TRACK_TYPE_FREE])
        {
            mas_impl_log(MAS_TEXT("MEMORY_LEAK[ 0x%p ]: %s -> [ %u ] - %s\n"), Alloc->Data,  Alloc->Source, Alloc->Line[TRACK_TYPE_ALLOCATE], Alloc->File[TRACK_TYPE_ALLOCATE]);
            LeakCount++;
        }
    }

    if(LeakCount > 0)
    {
        mas_impl_log(MAS_TEXT("\n    ::MEMORY_LEAK_COUNT: %u\n\n"), LeakCount);
        assert(LeakCount == 0 && "MEMORY_LEAK_DETECTED");
    }
}

