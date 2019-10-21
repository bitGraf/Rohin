#include "MemoryManager.hpp"

PoolAllocator::PoolAllocator(u16 numElements, u16 elementSize) :
    m_numElements(numElements), 
    m_elementSize(elementSize) {
    data = nullptr;
    freeList = nullptr;
}

PoolAllocator::~PoolAllocator() {
}

void PoolAllocator::create() {
    // create a block of memory to use
    data = (MemoryBlock*)calloc(m_numElements, m_elementSize *sizeof(MemoryBlock));
    // |----|----|----|----|

    for (u32 n = 0; n < m_numElements -1; n++) {
        u32* ptr = data + (n*m_elementSize);
        u32* next = ptr + m_elementSize;

        *ptr = reinterpret_cast<u32>(next);
    }

    freeList = data;
}

void PoolAllocator::destroy() {
    free(data);
    for (int n = 0; n < m_numElements*m_elementSize; n++) {
        *(data+n) = 0;
    }
    //elements = nullptr;
}

void PoolAllocator::freeDataBlock(MemoryBlock* block) {
    u32* ptr = freeList;

    *block = reinterpret_cast<u32>(ptr);
    freeList = block;
}

MemoryBlock* PoolAllocator::getBlock() {
    // Find next free block and take it.
    u32* ptr = freeList;
    u32* next = reinterpret_cast<u32*>(*ptr);
    freeList = next;

    for (int n = 0; n < m_elementSize; n++) {
        *(ptr+n) = 0;
    }

    return ptr;
}