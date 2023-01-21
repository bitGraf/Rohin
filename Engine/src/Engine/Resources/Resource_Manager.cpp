#include "Resource_Manager.h"

#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Core/Asserts.h"

global_variable memory_arena* resource_arena;

bool32 resource_init(memory_arena* arena) {
    resource_arena = arena;

    resource_arena->Size;

    return true;
}

void resource_shutdown() {
    ResetArena(resource_arena);
}

memory_arena* resource_get_arena() {
    Assert(resource_arena);

    return resource_arena;
}