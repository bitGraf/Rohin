#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

#include "Engine/Core/Base.hpp"

struct memory_arena {
    memory_index Size;
    uint8* Base;
    memory_index Used;
};

void InitializeArena(memory_arena* Arena, memory_index Size, uint8* Base);
void ClearArena(memory_arena* Arena);

#define SwapArenas(Arena1, Arena2) {memory_arena tmp = Arena1;Arena1 = Arena2;Arena2 = tmp;}

#define PushStruct(Arena, type) (type*)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, type, count) (type*)PushSize_(Arena, (count)*sizeof(type))
void* PushSize_(memory_arena* Arena, memory_index Size);

//struct temporary_arena {
//    memory_arena* Arena;
//    memory_index LastPos;
//};

//temporary_arena BeginTempArena(memory_arena* Arena);
//void EndTempArena(temporary_arena);

//temporary_arena GetScratch(void);
//#define ReleaseScratch(t) EndTempArena(t)

#endif