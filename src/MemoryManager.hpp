#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "DataTypes.hpp"
#include <stdlib.h>
#include <stdio.h>

template<typename dataType>
struct DataBlock {
    const size_t m_elementSize;
    u32 m_numElements;
    dataType* data;

    DataBlock(u32 num) : 
        m_elementSize(sizeof(dataType)),
        m_numElements(num),
        data(nullptr)
    {}
};

struct vec3 {
    u32 x[3];
};

/**
 * @class PoolAllocator
 * 
 * @brief Two-Sided Memory Pool
 * 
 * This class represents a memory pool allocator.
 * The allocator secures a large chunk of data at creation,
 * then passes out DataBlock objects when asked. This is done 
 * to allow run-time dynamic memory allocation without incurring
 * the heavy performance toll of using malloc() in real time.
 * 
 * This is a two-sided pool, meaning the memory chunk can be
 * pulled from both sides independently, eventually meeting 
 * in the middle.
 * 
 * Usage:
 * {@code
 *  PoolAllocator(80) pool; // Create a memory pool with a size of 80 bytes.
 * 
 *  pool.create(); // Allocate the memory needed from the system. Calls malloc() once.
 * 
 *  MemoryBlock<u32> block1 = pool.allocBlock<u32>(10);         // This pulls 40 bytes off the front. There are 40 bytes left.
 *  MemoryBlock<f32> block2 = pool.allocBlock<f32>(8, false);   // This pulls 32 bytes off the back.  There are  8 bytes left.
 *  MemoryBlock<f32> block3 = pool.allocBlock<f32>(5, false);   // This fails as there are only 8 bytes left in the pool. block3.data is nullptr
 *  MemoryBlock<f64> block4 = pool.allocBlock<f64>(1, true);   // This succeeds, pulling 8 bytes off the front. There are 0 bytes left.
 * 
 *  if (block1.data) { // check if block was created correctly
 *      useData(block1.data, block1.m_elementSize, block1.m_numElements); // use the data however is needed
 *  }
 * 
 *  ...
 * 
 *  // Return all the blocks to the pool
 *  pool.returnBlock(block1); 
 *  pool.returnBlock(block2);
 *  pool.returnBlock(block3);   // Even failed blocks
 *  pool.returnBlock(block4);
 * 
 *  pool.destroy();     // Destroy the pool allocator, freeing the entire block of memory back to the operating system.
 * 
 * }
 */
class PoolAllocator {
public:
    /**
     * @brief Construct PoolAllocator
     * 
     * Construct a Two-Sided Memory Pool Allocator, 
     * choosing the size at construction.
     * 
     * @warning Does not initialize data of pool, must call {@link create} first
     * @param howManyBytes u32 Size of the memory pool. Must be non-zero. Constant once set
     */
    PoolAllocator(const u32 howManyBytes);
    ~PoolAllocator(); ///< Empty destructor
    
    /**
     * @brief Initaize the allocator
     * 
     * Allocate the entire memory block and setup front and back
     * pointers. Must be called before requesting data.
     * 
     * @todo possible return error code if failed
     */
    void create();

    /**
     * @brief Destroys the whole pool, freeing all memory in the process
     * 
     * Destroys the whole pool, freeing all memory in the process.
     * Be careful to only call this when all blocks no longer referenced.
     * 
     * @todo put in a check to make sure all blocks are not referenced anymore
     */
    void destroy();

    /**
     * @brief Request a raw chunk of data (void pointer)
     * 
     * 
     * Request a raw pointer to a block of memory. The type is a void pointer
     * When using this method, specify the number of bytes you want.
     * 
     * @depreciated Use {@link allocBlock} instead
     * 
     * @param howManyBytes Requested size of block
     * @param pullFromFront bool Pull from front or back of pool (Default front)
     * @return void Pointer to dataBlock. nullptr if failed to request
     */
    void* allocBlock_raw(u32 howManyBytes, bool pullFromFront = true);

    /**
     * @brief Request memory block of desired type
     * 
     * Request a block of memory of a specified type. The type is specified
     * as a template, and the element size is automatically set at construction.
     * When using this method, specify the number of elements you want.
     * 
     * @tparam T Data type of the memory block
     * @param howManyElements How many elements of T you want
     * @param pullFromFront bool Pull from front or back of pool (Default front)
     * @return DataBlock<T> A DataBlock struct, that contains the raw
     * data, the size of each element, and the number of elemetns
     */
    template<typename T>
    DataBlock<T> allocBlock(u32 howManyElements, bool pullFromFront = true);

    /**
     * @brief Return a memory block to the pool
     * 
     * Return a memory block to the pool.
     * 
     * @depreciated Use {@link returnBlock} instead
     * 
     * @param block 
     */
    void  returnBlock_raw(void* block);

    /**
     * @brief Return a memory block to the pool
     * 
     * Return a memory block to the pool. The data pointer in the block
     * you pass in is set to nullptr.
     * 
     * @todo Add the returned block ~back~ to the pool , allowing it to be used again
     * 
     * @tparam T Data type of the memory block (Inherited from block)
     * @param block The memory block to free
     */
    template<typename T>
    void returnBlock(DataBlock<T>& block);

    /**
     * @brief Get size of this pool
     * 
     * Get the total size of this pool in bytes
     * 
     * @return u32 Total size of pool in bytes
     */
    u32 getTotalBytes() { return m_totalBytesInPool; }

    /**
     * @brief Get bytes left in pool
     * 
     * Get the remaining size of this pool in bytes
     * 
     * @return u32 Remaining size of pool in bytes
     */
    u32 getBytesLeft() { return m_bytesLeft; }

private:
    const u32 m_totalBytesInPool; ///< Size of memory pool (in bytes)
    u32 m_bytesLeft; ///< Bytes left in the center

    void* m_frontPointer; ///< Pointer to the front side of the pool
    void* m_backPointer; ///< Pointer to the back side of the pool
    void* m_rawData; ///< Raw pointer to entire pool of data

    /**
     * @brief Offset a void pointer a number of bytes
     * 
     * Offset a void pointer a number of bytes.
     * Since a void* cannot be incremented normally,
     * first it is cast to an unsigned 8bit integer.
     * Then it is incremented, before casting back to a 
     * void pointer
     * 
     * @param start Root pointer
     * @param numBytes Bytes to offset from start
     * @return void* Returns the offset pointer
     */
    void* byteOffset(void* start, s32 numBytes);
};

class MemoryManager {
public:

private:
    PoolAllocator allocator;

};

#endif