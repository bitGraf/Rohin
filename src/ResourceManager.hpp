#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <cassert>

#include "DataTypes.hpp"
#include "FileSystem.hpp"

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

private:
    FileSystem*     m_FileSystem;
    MessageBus*     m_msgBus;
    Console*        m_console;
    //MemoryManager*  m_memoryManager;

    ResourceList    m_resourceList;
    ResourceList*   m_tail;

    void freeResource(Resource* res);
};

#endif
