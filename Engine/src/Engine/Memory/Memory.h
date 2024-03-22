#pragma once

#include "Engine/Defines.h"

struct memory_arena;

// defined by platform files.
RHAPI void* memory_zero(void* memory, uint64 size);
RHAPI void* memory_copy(void* dest, const void* src, uint64 size);
RHAPI void* memory_set(void* memory, uint8 value, uint64 size);

// dynamic array
#define CreateArray(Arena, type, count) (type*)_CreateArraySize_(Arena, sizeof(type), count)
RHAPI void* _CreateArraySize_(memory_arena* arena, uint64 element_size, uint64 num_to_reserve);

// dynarray MUST be created (from CreateArray or internal) before using any of the below methods.
#define ArrayPushValue(dynarray, val)                                               \
{                                                                                   \
    decltype(val) temp = val;                                                       \
    dynarray = (decltype(dynarray))ArrayPushPtr(dynarray, &temp, sizeof(temp));   \
}
RHAPI void* ArrayPushPtr(void* dynarray, void* data_ptr, uint64 data_size);

#define ArrayAdd(dynarray) \
    dynarray = (decltype(dynarray))_ArrayAdd_(dynarray)
RHAPI void* _ArrayAdd_(void* dynarray);

#define ArrayResize(dynarray, new_count) \
    dynarray = (decltype(dynarray))_ArrayResize_(dynarray, new_count)
RHAPI void* _ArrayResize_(void* dynarray, uint64 new_count);

#define ArrayReserve(dynarray, new_capacity) \
    dynarray = (decltype(dynarray))_ArrayReserve_(dynarray, new_capacity)
RHAPI void* _ArrayReserve_(void* dynarray, uint64 new_capacity);

#define ArrayPeek(dynarray) \
    (decltype(dynarray))_ArrayPeek_(dynarray)
RHAPI void* _ArrayPeek_(void* dynarray);

RHAPI void ArrayClear(void* dynarray);

RHAPI uint64 GetArrayCount(void* dynarray);
RHAPI uint64 GetArrayStride(void* dynarray);
RHAPI uint64 GetArrayCapacity(void* dynarray);
RHAPI memory_arena* GetArrayArena(void* dynarray);