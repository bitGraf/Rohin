#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <cassert>
#include <unordered_map>
#include <dirent.h>

#include "Scene/Material.hpp"
#include "FileSystem.hpp"
#include "MemoryManager.hpp"
#include "TriangleMesh.hpp"

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
    MemoryPool m_pool;

    std::unordered_map<std::string, TriangleMesh> meshes;
    std::unordered_map<std::string, Material> materials;

    void initializeTriangleMesh(TriangleMesh* mesh);
    void initializeMaterial(Material* mat);
    void genTextureFromData(Material_Texture* tex);
};

#endif
