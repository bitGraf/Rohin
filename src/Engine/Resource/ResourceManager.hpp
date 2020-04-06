#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <cassert>
#include <unordered_map>
#include <dirent.h>

#include "Scene/Material.hpp"
#include "Message/CoreSystem.hpp"
#include "FileSystem.hpp"
#include "MemoryManager.hpp"
#include "TriangleMesh.hpp"

//#include "stb_image.h"

const u32 KILOBYTE = 1024;              // 1 KB worth of bytes
const u32 MEGABYTE = 1024 * KILOBYTE;   // 1 MB worth of bytes

class ResourceManager : public CoreSystem {
public:
    ResourceManager();
    ~ResourceManager();

    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    CoreSystem* create();
    void setRootDirectory(char* exeLoc);

    void loadResourceFile(std::string);

    template <typename TYPE>
    DataBlock<TYPE> reserveDataBlocks(int num) {
        return m_pool.allocBlock<TYPE>(num);
    }

private:
    FileSystem m_FileSystem;
    PoolAllocator m_pool;
};

#endif
