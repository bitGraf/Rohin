#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

#include "Engine/Core/Base.hpp"

struct memory_arena {
    memory_index Size;
    uint8* Base;
    memory_index Used;
};

void InitializeArena(memory_arena* Arena, memory_index Size, uint8* Base);

#define PushStruct(Arena, type) (type*)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, type, count) (type*)PushSize_(Arena, (count)*sizeof(type))
void* PushSize_(memory_arena* Arena, memory_index Size);

#endif