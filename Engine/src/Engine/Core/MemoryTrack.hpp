#pragma once

#define TRACK_NEW_AND_DELETE 0

namespace rh {

    namespace MemoryTracker {
        void Alloc(size_t sz);
        void Free(size_t sz);

        void PrintMemoryUsage();
    };
}