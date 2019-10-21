#include "ResourceManager.hpp"

ResourceManager::ResourceManager() {
    m_FileSystem = nullptr;
    m_msgBus = nullptr;
    m_console = nullptr;
}

ResourceManager::~ResourceManager() {
}

void ResourceManager::create() {
    m_resourceList.resource.data = nullptr;
    m_resourceList.next = nullptr;

    m_tail = &m_resourceList;
}

void ResourceManager::destroy() {
    ResourceList* head = m_resourceList.next;
    ResourceList* next = nullptr;

    while (head != nullptr) {
        // save pointer to next element 
        // before deleting this element
        next = head->next;

        freeResource(&head->resource);
        free(head);

        head = next;
        next = nullptr;
    }

    // done stepping through the list
    // free the root resource.
    freeResource(&m_resourceList.resource);
}

void ResourceManager::setMessageBus(MessageBus* _messageBus) {
    m_msgBus = _messageBus;
}

void ResourceManager::setFileSystem(FileSystem* _filesys) {
    m_FileSystem = _filesys;
}

void ResourceManager::setConsole(Console* _console) {
    m_console = _console;
}

/*void ResourceManager::setMemoryManager(MemoryManager* _memManager) {
    m_memoryManager = _memManager;
}*/

void ResourceManager::createNewResource(stringID id) {
    ResourceList* res = new ResourceList;
    res->next = nullptr;
    res->resource.data = nullptr;
    res->resource.ID = id;

    assert(m_tail->next == nullptr);

    m_tail->next = res;
    m_tail = m_tail->next;
}

void ResourceManager::printAllResources() {
    int numResources = 0;
    ResourceList* next = m_resourceList.next;
    while (next != nullptr) {
        // next is a valid entry in the list
        printf("ID: %d\n", next->resource.ID);

        next = next->next;
        
        numResources++;
    }
    printf("%d resources cycled.\n", numResources);
}

void ResourceManager::freeResource(Resource* res) {
    res->ID = 0;
    free(res->data);
    res->data = nullptr;
}