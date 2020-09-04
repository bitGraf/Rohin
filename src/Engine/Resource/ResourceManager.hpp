#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <cassert>
#include <unordered_map>
#include <dirent.h>

#include "Scene/Material.hpp"
#include "FileSystem.hpp"
#include "MemoryManager.hpp"
#include "TriangleMesh.hpp"

const u32 KILOBYTE = 1024;              // 1 KB worth of bytes
const u32 MEGABYTE = 1024 * KILOBYTE;   // 1 MB worth of bytes

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    void update(double dt);
    void destroy();
    void setRootDirectory(char* exeLoc);

    void loadResourceFile(std::string);

    template <typename TYPE>
    DataBlock<TYPE> reserveDataBlocks(int num) {
        return m_pool.allocBlock<TYPE>(num);
    }

    meshRef getMesh(std::string);
    materialRef getMaterial(std::string);

private:
    FileSystem m_FileSystem;
    PoolAllocator m_pool;

    std::unordered_map<std::string, TriangleMesh> meshes;
    std::unordered_map<std::string, Material> materials;

    void initializeTriangleMesh(TriangleMesh* mesh);
    void initializeMaterial(Material* mat);
    void genTextureFromData(Material_Texture* tex);
};

#endif
