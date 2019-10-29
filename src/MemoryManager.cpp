#include "MemoryManager.hpp"

// Explicit instantiation for various types
template void PoolAllocator::returnBlock(DataBlock<f32>&);
template void PoolAllocator::returnBlock(DataBlock<f64>&);
template void PoolAllocator::returnBlock(DataBlock<math::vec3>&);

// Explicit instantiation for various types
template DataBlock<f32> PoolAllocator::allocBlock(u32, bool);
template DataBlock<f64> PoolAllocator::allocBlock(u32, bool);
template DataBlock<math::vec3> PoolAllocator::allocBlock(u32, bool);
template DataBlock<math::vec2> PoolAllocator::allocBlock(u32, bool);
template DataBlock<index_t> PoolAllocator::allocBlock(u32, bool);

// Explicit instantiation for various types
template DataBlock<math::vec3>& DataBlock<math::vec3>::operator=(const DataBlock<math::vec3>& db);
template DataBlock<math::vec2>& DataBlock<math::vec2>::operator=(const DataBlock<math::vec2>& db);
template DataBlock<index_t>& DataBlock<index_t>::operator=(const DataBlock<index_t>& db);

/* Assignment Operator */
template<typename dataType>
DataBlock<dataType>& DataBlock<dataType>::operator=(const DataBlock<dataType>& db) {
    assert(this->m_elementSize == db.m_elementSize);
    assert(db.data != nullptr);
    assert(db.m_numElements > 0);

    this->m_numElements = db.m_numElements;
    this->data = db.data;

    return *this;
}

PoolAllocator::PoolAllocator(const u32 howManyBytes) :
    m_totalBytesInPool(howManyBytes),
    m_bytesLeft(howManyBytes) {

    m_rawData = nullptr;

    m_frontPointer = nullptr;
    m_backPointer = nullptr;
}

PoolAllocator::~PoolAllocator() {
}

void PoolAllocator::create() {
    m_rawData = malloc(m_totalBytesInPool);

    if (m_rawData) {
        // successfully alloc'd the data

        m_frontPointer = m_rawData;
        m_backPointer = byteOffset(m_frontPointer, m_totalBytesInPool);
    }
    else {
        printf("Error, failed to allocate %d bytes of data.\n", 
            m_totalBytesInPool);
    }
}

void PoolAllocator::destroy() {
    if (m_rawData) {
        free(m_rawData);
    }
    m_rawData = nullptr;

    m_frontPointer = nullptr;
    m_backPointer = nullptr;

    m_bytesLeft = 0;
}

void* PoolAllocator::allocBlock_raw(u32 howManyBytes, bool pullFromFront) {
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

template<typename T>
DataBlock<T> PoolAllocator::allocBlock(u32 howManyElements, bool pullFromFront) {
    DataBlock<T> ret(howManyElements);

    u32 howManyBytes = (howManyElements*ret.m_elementSize);
    if (m_rawData && m_bytesLeft >= howManyBytes) {
        T* start = nullptr;

        if (pullFromFront) {
            start = static_cast<T*>(m_frontPointer);
            m_frontPointer = static_cast<void*>(start + howManyElements);
        }
        else {
            start = static_cast<T*>(m_backPointer) - howManyElements;
            m_backPointer = static_cast<void*>(start);
        }

        m_bytesLeft -= howManyBytes;

        ret.data = start;
        return ret;
    }

    ret.m_numElements = 0;
    ret.data = nullptr;
    return ret;
}

void PoolAllocator::returnBlock_raw(void* block) {
    // don't really need to do anything right now, as the
    // allocator doesn't return the data back to the pool yet
    block = nullptr;
}

template<typename T>
void PoolAllocator::returnBlock(DataBlock<T>& block) {
    block.data = nullptr;
    block.m_numElements = 0;
}

void* PoolAllocator::byteOffset(void* start, s32 numBytes) {
    // First cast to u8 (one-byte type)
    u8* ptr = static_cast<u8*>(start);

    // Add byte offset to pointer
    ptr += numBytes;

    // cast back to void pointer
    return static_cast<void*>(ptr);

    /* in one disgusting line */
    //return static_cast<void*>(static_cast<u8*>(start) + numBytes);
}