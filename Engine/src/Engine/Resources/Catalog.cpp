#include <enpch.hpp>

#include "Catalog.hpp"

namespace Engine {

    bool is_power_of_two(uintptr_t x);
    uintptr_t align_forward(uintptr_t ptr, size_t align);

    void Catalog::init(void* backing_buffer, size_t backing_buffer_length, size_t block_size, size_t block_align) {
        buffer = (unsigned char*)backing_buffer;
        buffer_length = backing_buffer_length;
        size = block_size;
        align = block_align;
        curr_offset = 0;
    }

    void* Catalog::alloc() {
        uintptr_t curr_ptr = (uintptr_t)buffer + (uintptr_t)curr_offset;
        uintptr_t offset = align_forward(curr_ptr, align);
        offset -= (uintptr_t)buffer;

        // see if there is any memory left
        if (offset + size <= buffer_length) {
            void* ptr = &buffer[offset];
            curr_offset = offset + size;

            // zero new memory by default
            memset(ptr, 0, size);
            return ptr;
        }

        // arena is out of memory, return null
        return nullptr;
    }

    void Catalog::free_all() {
        curr_offset = 0;
    }

    bool is_power_of_two(uintptr_t x) {
        return (x & (x - 1)) == 0;
    }

    uintptr_t align_forward(uintptr_t ptr, size_t align) {
        uintptr_t p, a, modulo;

        assert(is_power_of_two(align));

        p = ptr;
        a = (uintptr_t)align;
        // Same as (p % a) but faster as 'a' is a power of two
        modulo = p & (a - 1);

        if (modulo != 0) {
            // If 'p' address is not aligned, push the address to the
            // next value which is aligned
            p += a - modulo;
        }
        return p;
    }

}