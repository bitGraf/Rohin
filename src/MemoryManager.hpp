#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "DataTypes.hpp"
#include <stdlib.h>

typedef u32 MemoryBlock; // a generic datablock

class PoolAllocator {
public:
    PoolAllocator(u16 numElements, u16 elementSize);
    ~PoolAllocator();
    
    void create();
    void destroy();

    MemoryBlock* getBlock();
    void freeDataBlock(MemoryBlock* block);

    MemoryBlock* data; // Pointer to entire data block
private:
    const u16 m_numElements, const m_elementSize;
    MemoryBlock* freeList; // Pointer to head of free memory
};

class MemoryManager {
public:

private:
    PoolAllocator allocator;

};

#endif