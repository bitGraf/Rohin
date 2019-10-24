#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <cassert>

#include "DataTypes.hpp"
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

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    void create();
    void destroy();
    void setConsole(Console* console);
    void setFileSystem(FileSystem* _filesys);
    void setMessageBus(MessageBus* _messageBus);
    //void setMemoryManager(MemoryManager* _memManager);

    void createNewResource(stringID id = 0);
    void printAllResources();

    void loadModelFromFile(std::string path);

private:
    FileSystem*     m_FileSystem;
    MessageBus*     m_msgBus;
    Console*        m_console;
    //MemoryManager*  m_memoryManager;

    ResourceList    m_resourceList;
    ResourceList*   m_tail;

    tinygltf::TinyGLTF loader;

    void* readAccessor(tinygltf::Model* root, int accessorID);
    void* readImage(tinygltf::Model* root, int imageID);

    void freeResource(Resource* res);
};

#endif
