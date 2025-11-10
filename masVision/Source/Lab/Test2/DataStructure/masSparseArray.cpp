#include <stdint.h>
#include <stdlib.h>
#include <string.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAS_PTR_OFFSET(type, ptr, offset) (type*)(((uint8_t*)ptr) + offset)
#define MAS_SPARSE_ARRAY_NAME_SIZE 128


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef void (*masSparseArrayFunc_DataDestructor)(void* Data);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef union _masID
{
    uint64_t Signature;
    struct
    {
        uint32_t MapperIdx;
        uint16_t Generation;
        uint16_t RegisterID;
    };
} masID;
// api for masID to get its info like pool's name it belong, is_valid, ..

typedef struct _masIDMapper
{
    uint32_t DataIdx;
    uint16_t Generation;
    uint16_t Reserved;
} masIDMapper;

typedef struct _masSparseArray
{
    masSparseArrayFunc_DataDestructor DataDestructor;
    masIDMapper* IDsMapper;
    int32_t* FreeMappers;
    uint8_t* Data;
    int32_t      FreeCount;
    int32_t      AllocIdx;
    int32_t      Capacity;
    int32_t      UsedCount;
    int32_t      ResizeCounter;
    int32_t      RegisterID;
    int32_t      ElementSize;
    char         Name[MAS_SPARSE_ARRAY_NAME_SIZE];
} masSparseArray;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool masInternalSparseArray_Resize(masSparseArray** SparseArrayPtr)
{
    // dont forget after resize to updated registery

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
masSparseArray* masSparseArray_Create(const char* RegisterName, int32_t ElementSize, int32_t Capacity, masSparseArrayFunc_DataDestructor DataDestructor)
{
    if (ElementSize <= 0 || Capacity <= 0)
        return NULL;

    int32_t IDsMapperSize = sizeof(masIDMapper) * Capacity;
    int32_t FreeMappersSize = sizeof(int32_t) * Capacity;
    int32_t DataSize = ElementSize * Capacity;
    int32_t MemorySize = sizeof(masSparseArray) + IDsMapperSize + FreeMappersSize + DataSize;
    masSparseArray* SparseArray = (masSparseArray*)malloc(MemorySize);
    if (!SparseArray)
        return NULL;
    memset(SparseArray, 0, MemorySize);

    SparseArray->IDsMapper = MAS_PTR_OFFSET(masIDMapper, SparseArray, sizeof(masSparseArray));
    SparseArray->Data = MAS_PTR_OFFSET(uint8_t, SparseArray->IDsMapper, IDsMapperSize);
    SparseArray->Capacity = Capacity;
    SparseArray->ElementSize = ElementSize;
    SparseArray->DataDestructor = DataDestructor;

    if (RegisterName)
    {
        int32_t NameLen = strlen(RegisterName);
        if (NameLen >= MAS_SPARSE_ARRAY_NAME_SIZE)
            NameLen = MAS_SPARSE_ARRAY_NAME_SIZE - 1;
        memcpy(SparseArray->Name, RegisterName, NameLen);

        //SparseArray->RegisterID = masRegistery_RegisterSparseArray(RegisterName, SparseArray); // not sure if name would be stored twice in registery and sparse array
    }

    return SparseArray;
}

void masSparseArray_Destroy(masSparseArray** SparseArrayPtr)
{
    if (!SparseArrayPtr && !(*SparseArrayPtr))
        return;

    masSparseArray* SparseArray = *SparseArrayPtr;
    if (SparseArray->DataDestructor && SparseArray->Data && SparseArray->ElementSize > 0)
    {
        // UsedCount assuming that data is always packed and no gap exist between data
        for (int32_t i = 0; i < SparseArray->UsedCount; ++i)
        {
            void* Data = MAS_PTR_OFFSET(void, SparseArray->Data, SparseArray->ElementSize * i);
            SparseArray->DataDestructor(Data);
        }
    }

    //masRegistery_UnRegisterSparseArray(*SparseArrayPtr);

    free(*SparseArrayPtr);
    *SparseArrayPtr = NULL;
}

masID masSparseArray_Alloc(masSparseArray** SparseArrayPtr)
{
    if (!SparseArrayPtr || !(*SparseArrayPtr))
        return { 0 };

}

void masSparseArray_Free(masSparseArray* SparseArray, masID ID)
{
    if (!SparseArray || SparseArray->RegisterID != ID.RegisterID || ID.Signature == 0 || ID.Generation == 0)
        return;

    // remove and swap it with the end upon removal and update masIDMapper.DataIdx and add mapper idx to freemappers list
}

void* masSparseArray_GetData(masSparseArray* SparseArray, masID ID)
{
    if (!SparseArray || SparseArray->RegisterID != ID.RegisterID || ID.Signature == 0 || ID.Generation == 0)
        return NULL;

    masIDMapper* IDMapper = &SparseArray->IDsMapper[ID.MapperIdx];
    if (IDMapper->Generation != ID.Generation)
        return NULL;

    void* Data = MAS_PTR_OFFSET(void, SparseArray->Data, IDMapper->DataIdx * SparseArray->ElementSize);
    return Data;
}

int32_t     masSparseArray_Capacity(masSparseArray* SparseArray)      { if (!SparseArray) return 0;    return SparseArray->Capacity;      }
int32_t     masSparseArray_UsedCount(masSparseArray* SparseArray)     { if (!SparseArray) return 0;    return SparseArray->UsedCount;     }
int32_t     masSparseArray_RegisterID(masSparseArray* SparseArray)    { if (!SparseArray) return 0;    return SparseArray->RegisterID;    }
int32_t     masSparseArray_ElementSize(masSparseArray* SparseArray)   { if (!SparseArray) return 0;    return SparseArray->ElementSize;   }
int32_t     masSparseArray_ResizeCounter(masSparseArray* SparseArray) { if (!SparseArray) return 0;    return SparseArray->ResizeCounter; }
const char* masSaprseArray_Name(masSparseArray* SparseArray)          { if (!SparseArray) return NULL; return SparseArray->Name;          }