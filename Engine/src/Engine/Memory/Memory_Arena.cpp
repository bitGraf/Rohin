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
    AssertMsg((Arena->Used + Size) <= Arena->Size, "Arena ran out of memory");
    void* Result = Arena->Base + Arena->Used;
    Arena->Used += Size;

    return Result;
}

memory_arena CreateSubArena(memory_arena* BaseArena, memory_index SubArenaSize) {
    AssertMsg((BaseArena->Size - BaseArena->Used) >= SubArenaSize, "SubArena is too large");
    memory_arena sub_arena;

    CreateArena(&sub_arena, SubArenaSize, BaseArena->Base + BaseArena->Used);
    PushSize_(BaseArena, SubArenaSize);

    return sub_arena;
}