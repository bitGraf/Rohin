#include "Memory.h"

#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Core/Asserts.h"

// the data (i.e. void*) starts at offset 0, so it can be used as a normal array
// these offsets are negative, so they are stored 'before' the data
const int64 DYNARRAY_ARENA    = -4;
const int64 DYNARRAY_CAPACITY = -3;
const int64 DYNARRAY_STRIDE   = -2;
const int64 DYNARRAY_COUNT    = -1;
const int64 DYNARRAY_DATA     =  0;
constexpr uint64 HEADER_SIZE = 4*sizeof(int64);

#define CALC_NEW_CAP(cap) (((cap)*3U)/2U)

void* _CreateArraySize_(memory_arena* arena, uint64 element_size, uint64 num_to_reserve) {
    uint64 array_size = HEADER_SIZE + (element_size * num_to_reserve);
    void* total = PushSize_(arena, array_size);
    memory_zero(total, array_size);

    void* dynarray = (void*)(((uint64*)total) + 4);

    ((uint64*)dynarray)[DYNARRAY_COUNT] = 0; // start with 0 elements
    ((uint64*)dynarray)[DYNARRAY_STRIDE] = element_size;
    ((uint64*)dynarray)[DYNARRAY_CAPACITY] = num_to_reserve;
    ((memory_arena**)dynarray)[DYNARRAY_ARENA] = arena;

    return dynarray;
}

void* ArrayPushPtr(void* dynarray, void* data_ptr, uint64 data_size) {
    uint64 count    = ((uint64*)dynarray)[DYNARRAY_COUNT];
    uint64 stride   = ((uint64*)dynarray)[DYNARRAY_STRIDE];
    uint64 capacity = ((uint64*)dynarray)[DYNARRAY_CAPACITY];
    memory_arena* arena = ((memory_arena**)dynarray)[DYNARRAY_ARENA];

    Assert(data_size <= stride && "Tried to push too many bytes to a dynamic array!");

    if ((count+1) > capacity) {
        // Need to auto resize!
        uint64 new_capacity = CALC_NEW_CAP(capacity);
        dynarray = _ArrayReserve_(dynarray, new_capacity);
    }

    // There is enough space
    uint8* insert_at = ((uint8*)dynarray + (count*stride));
    memory_copy(insert_at, data_ptr, stride);
    ((uint64*)dynarray)[DYNARRAY_COUNT]++; // increment count

    return dynarray;
}

void* _ArrayResize_(void* dynarray, uint64 new_count) {
    uint64 count    = ((uint64*)dynarray)[DYNARRAY_COUNT];
    uint64 stride   = ((uint64*)dynarray)[DYNARRAY_STRIDE];
    uint64 capacity = ((uint64*)dynarray)[DYNARRAY_CAPACITY];
    memory_arena* arena = ((memory_arena**)dynarray)[DYNARRAY_ARENA];

    // don't need to resize, just set the count to the new valueT
    if (new_count < count) {
        ((uint64*)dynarray)[DYNARRAY_COUNT] = new_count;
        return dynarray;
    }

    // check if we need to resize the array
    if (new_count > capacity) {
        void* old_dynarray = dynarray;

        uint64 new_capacity = CALC_NEW_CAP(capacity);
        dynarray = _ArrayReserve_(dynarray, new_capacity);
    }

    // we have enough capacity now, set the new size
    // zero out the new elements as well.
    uint8* end = (uint8*)(dynarray) + count*stride;
    uint64 zero_size = (new_count - count)*stride;

    ((uint64*)dynarray)[DYNARRAY_COUNT] = new_count;
    memory_zero(end, zero_size);

    return dynarray;
}
void* _ArrayReserve_(void* dynarray, uint64 new_capacity) {
    uint64 count    = ((uint64*)dynarray)[DYNARRAY_COUNT];
    uint64 stride   = ((uint64*)dynarray)[DYNARRAY_STRIDE];
    uint64 capacity = ((uint64*)dynarray)[DYNARRAY_CAPACITY];
    memory_arena* arena = ((memory_arena**)dynarray)[DYNARRAY_ARENA];

    if (new_capacity > capacity) {
        void* old_dynarray = dynarray;

        // need to reallocate
        dynarray = _CreateArraySize_(arena, stride, new_capacity);

        // copy any existing data
        memory_copy(dynarray, old_dynarray, count * stride);
        ((uint64*)dynarray)[DYNARRAY_COUNT] = count;
    }

    return dynarray;
}

void* _ArrayPeek_(void* dynarray) {
    uint64 count    = ((uint64*)dynarray)[DYNARRAY_COUNT];
    uint64 stride   = ((uint64*)dynarray)[DYNARRAY_STRIDE];
    uint64 capacity = ((uint64*)dynarray)[DYNARRAY_CAPACITY];
    memory_arena* arena = ((memory_arena**)dynarray)[DYNARRAY_ARENA];

    Assert(count > 0 && "Tried to pop from an array with count 0");

    // get last element
    uint64 offset = (count-1) * stride;
    return ((uint8*)dynarray) + offset;
}

void ArrayClear(void* dynarray) {
    // just sets the count to zero, does NOT zero out memory, or adjust capacity.
    ((uint64*)dynarray)[DYNARRAY_COUNT] = 0;
}

// Access header members
uint64 GetArrayCount(void* darray) {
    return ((uint64*)darray)[DYNARRAY_COUNT];
}

uint64 GetArrayStride(void* darray) {
    return ((uint64*)darray)[DYNARRAY_STRIDE];
}

uint64 GetArrayCapacity(void* darray) {
    return ((uint64*)darray)[DYNARRAY_CAPACITY];
}

memory_arena* GetArrayArena(void* darray) {
    return ((memory_arena**)darray)[DYNARRAY_ARENA];
}