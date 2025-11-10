#pragma once

#include <stdint.h>


typedef struct _masComponent
{
    int32_t id;
} masComponent;

// Serve as index into componentdesc pool
typedef struct _masComponentDescID
{
    int32_t id;
} masComponentDescID;

// Need Pool of this
typedef struct _masComponentDesc
{
    masComponentDescID Next;
    masComponentDescID Prev;
    uint32_t           Type;
} masComponentDesc;


// serve as index into entity desc pool
typedef struct _masEntity
{
    uint64_t id;
} masEntity;

// need pool for entities
typedef struct _masEntityDesc
{
    masComponentDescID Components;
    uint64_t           ComponentsHash;
    uint32_t           Version;
} masEntityDesc;


// EntityID(idx) -> EntityDesc(size_in_memory + first component desc meta) -> ComponentDescMetaData(type + ID) -> Internal per component pool(meshcomp, scenecomp, transformcomp, ...)