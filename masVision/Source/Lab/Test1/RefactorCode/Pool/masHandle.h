#pragma once

#include <stdint.h>

#define MAS_DECLARE_HANDLE(Type)\
    struct Type                 \
    {                           \
        uint32_t SlotIdx;       \
        uint32_t GenID;         \
    }
