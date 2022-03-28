#include <enpch.hpp>

#include "MemoryTrack.hpp"

#if TRACK_NEW_AND_DELETE
void* operator new(std::size_t sz) // no inline, required by [replacement.functions]/3
{
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success

    Engine::MemoryTracker::Alloc(sz);
    if (void *ptr = std::malloc(sz))
        return ptr;

    throw std::bad_alloc{}; // required by [new.delete.single]/3
}
void operator delete(void* ptr, std::size_t sz) noexcept
{
    std::free(ptr);
    Engine::MemoryTracker::Free(sz);
}
#endif

namespace Engine {

    namespace MemoryTracker {

        u64 s_totalMemoryAllocated = 0;
        u64 s_totalMemoryFreed = 0;
        u64 s_numAllocations = 0;

        void Alloc(size_t sz) {
            s_totalMemoryAllocated += sz;
            s_numAllocations++;
        }

        void Free(size_t sz) {
            s_totalMemoryFreed += sz;
        }

        void PrintMemoryUsage() {
            ENGINE_LOG_INFO("{0} Allocations, {1}/{2} bytes freed.", s_numAllocations, s_totalMemoryFreed, s_totalMemoryAllocated);
        }

    }

}