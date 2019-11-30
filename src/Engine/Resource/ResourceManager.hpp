#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <cassert>
#include <unordered_map>

#include "Scene/Material.hpp"
#include "Message/CoreSystem.hpp"
#include "FileSystem.hpp"
#include "MemoryManager.hpp"
#include "TriangleMesh.hpp"

#include "tiny_gltf.h"
#include "stb_image.h"

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

    void loadModelFromFile(std::string path, bool binary);

    template <typename TYPE>
    DataBlock<TYPE> reserveDataBlocks(int num) {
        return m_pool.allocBlock<TYPE>(num);
    }

    meshRef getMesh(std::string);
    materialRef getMaterial(std::string);

private:
    FileSystem m_FileSystem;
    //MemoryManager*  m_memoryManager;
    PoolAllocator m_pool;

    tinygltf::TinyGLTF loader;

    void* readAccessor(tinygltf::Model* root, int accessorID);
    void* readImage(tinygltf::Model* root, int imageID);

    std::unordered_map<std::string, TriangleMesh> meshes;
    std::unordered_map<std::string, Material> materials;
    std::vector<std::string> loadedResourceFiles;

    void processMesh(tinygltf::Model* root, int id);

    template<typename T>
    DataBlock<T> processAccessor(tinygltf::Model* root, int id);
    void initializeTriangleMesh(TriangleMesh* mesh);
    void initializeTexture(tinygltf::Model* root, Material_Texture* mTex);
};

#endif
