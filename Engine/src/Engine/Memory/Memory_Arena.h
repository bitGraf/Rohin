#pragma once

#include "Engine/Defines.h"

struct memory_arena {
    memory_index Size;
    uint8* Base;
    memory_index Used;
};

RHAPI void CreateArena(memory_arena* Arena, memory_index Size, uint8* Base);
RHAPI void ResetArena(memory_arena* Arena);

#define PushStruct(Arena, type) (type*)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, type, count) (type*)PushSize_(Arena, (count)*sizeof(type))
RHAPI void* PushSize_(memory_arena* Arena, memory_index Size);