#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <cassert>

#include "CoreSystem.hpp"
#include "FileSystem.hpp"

#include "tiny_gltf.h"

struct Resource {
    stringID ID;
    void* data;
};

struct ResourceList {
    Resource resource;
    ResourceList* next;
};

class ResourceManager : public CoreSystem {
public:
    ResourceManager();
    ~ResourceManager();

    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    void sys_create(ConfigurationManager* configMgr);

    void setFileSystem(FileSystem* _filesys);
    //void setMemoryManager(MemoryManager* _memManager);

    void createNewResource(stringID id = 0);
    void printAllResources();

    void loadModelFromFile(std::string path);

private:
    FileSystem*     m_FileSystem;
    //MemoryManager*  m_memoryManager;

    ResourceList    m_resourceList;
    ResourceList*   m_tail;

    tinygltf::TinyGLTF loader;

    void* readAccessor(tinygltf::Model* root, int accessorID);
    void* readImage(tinygltf::Model* root, int imageID);

    void freeResource(Resource* res);
};

#endif
