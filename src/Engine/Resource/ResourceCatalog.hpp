#ifndef RESOURCE_CATALOG_H
#define RESOURCE_CATALOG_H

#include "FileSystem.hpp"
#include "MemoryManager.hpp"
#include "Message/EMS.hpp"
#include "Resource/FileSystem.hpp"

#include <unordered_map>

const u32 KILOBYTE = 1024;              // 1 KB worth of bytes
const u32 MEGABYTE = 1024 * KILOBYTE;   // 1 MB worth of bytes

enum ResourceEntryType {
    resMesh,
    resImage,
    resFont,
    resShader,
    resOther
};

struct ResourceEntry {
    std::string filename;
    ResourceEntryType type;

    void* data;
    u32 size;
};

class ResourceCatalog : public MessageReceiver {
public:
    ResourceCatalog();
    ~ResourceCatalog();

    static ResourceCatalog* GetInstance();

    bool Init();
    void Destroy();

    void createNewResource(char* name, ResourceEntryType type, bool empty = false);
    bool getResource(std::string name, ResourceEntry& ent);

    void updateResourceFromFile(void* data, u32 size);

private:
    std::unordered_map<std::string, ResourceEntry> resources;

    static ResourceCatalog* _singleton;
};

#endif
