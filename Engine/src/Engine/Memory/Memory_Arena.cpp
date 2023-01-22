#include "Memory_Arena.h"

#include "Engine/Core/Asserts.h"

void CreateArena(memory_arena* Arena, memory_index Size, uint8* Base) {
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

void ResetArena(memory_arena* Arena) {
    Arena->Used = 0;
}

void* PushSize_(memory_arena* Arena, memory_index Size) {
    Assert((Arena->Used + Size) <= Arena->Size);
    void* Result = Arena->Base + Arena->Used;
    Arena->Used += Size;

    return Result;
}

memory_arena CreateSubArena(memory_arena* BaseArena, memory_index SubArenaSize) {
    Assert((BaseArena->Size - BaseArena->Used) >= SubArenaSize);
    memory_arena sub_arena;

    CreateArena(&sub_arena, SubArenaSize, BaseArena->Base + BaseArena->Used);
    PushSize_(BaseArena, SubArenaSize);

    return sub_arena;
}