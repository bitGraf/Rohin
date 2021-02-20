#include "rhpch.hpp"
#include "MemoryManager.hpp"

MemoryPool* MemoryPool::_singleton = 0;

MemoryPool* MemoryPool::GetInstance() {
    if (!_singleton) {
        _singleton = new MemoryPool(256 * MEGABYTE);
    }
    return _singleton;
}

MemoryPool::MemoryPool(const u32 howManyBytes) :
    m_totalBytesInPool(howManyBytes),
    m_bytesLeft(howManyBytes) {

    m_rawData = nullptr;

    m_frontPointer = nullptr;
    m_backPointer = nullptr;
}

MemoryPool::~MemoryPool() {
}

bool MemoryPool::Init() {
    BENCHMARK_FUNCTION();
    m_rawData = malloc(m_totalBytesInPool);

    if (m_rawData) {
        // successfully alloc'd the data

        m_frontPointer = m_rawData;
        m_backPointer = byteOffset(m_frontPointer, m_totalBytesInPool);
        return true;
    }
    else {
        printf("Error, failed to allocate %d bytes of data.\n", 
            m_totalBytesInPool);
        return false;
    }
}

void MemoryPool::Destroy() {
    BENCHMARK_FUNCTION();
    if (m_rawData) {
        free(m_rawData);
    }
    m_rawData = nullptr;

    m_frontPointer = nullptr;
    m_backPointer = nullptr;

    m_bytesLeft = 0;

    if (_singleton) {
        delete _singleton;
        _singleton = 0;
    }
}

void* MemoryPool::allocBlock_raw(u32 howManyBytes, bool pullFromFront) {
    if (m_rawData && m_bytesLeft >= howManyBytes) {
        void* start = nullptr;

        if (pullFromFront) {
            start = m_frontPointer;
            m_frontPointer = byteOffset(start, howManyBytes);
        } else {
            start = byteOffset(m_backPointer, -static_cast<s32>(howManyBytes));
            m_backPointer = start;
        }

        m_bytesLeft -= howManyBytes;

        return start;
    }

    return nullptr;
}

void MemoryPool::returnBlock_raw(void* block) {
    // don't really need to do anything right now, as the
    // allocator doesn't return the data back to the pool yet
    block = nullptr;
}

void* MemoryPool::byteOffset(void* start, s32 numBytes) {
    // First cast to u8 (one-byte type)
    u8* ptr = static_cast<u8*>(start);

    // Add byte offset to pointer
    ptr += numBytes;

    // cast back to void pointer
    return static_cast<void*>(ptr);

    /* in one disgusting line */
    //return static_cast<void*>(static_cast<u8*>(start) + numBytes);
}