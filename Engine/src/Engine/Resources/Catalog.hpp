#pragma once
#include <assert.h>
#include <cstring>

namespace Engine {

    struct Catalog {
        size_t buffer_length;
        size_t curr_offset;
        size_t size;
        size_t align;

        unsigned char* buffer;

        void init(void* backing_buffer, size_t backing_buffer_length, size_t block_size, size_t block_align);
        void* alloc();
        void free_all();
    };
}