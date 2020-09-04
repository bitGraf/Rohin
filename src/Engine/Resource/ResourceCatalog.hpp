#ifndef RESOURCE_CATALOG_H
#define RESOURCE_CATALOG_H

#include "FileSystem.hpp"
#include "MemoryManager.hpp"
#include "Message/EMS.hpp"
#include "Resource/FileSystem.hpp"

#include "TriangleMesh.hpp"
#include "Scene/Material.hpp"

#include <unordered_map>

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

    bool Init();
    void Destroy();

    bool loadResourceFile(std::string filename);

    void createNewResource(char* name, ResourceEntryType type, bool empty = false);
    bool getResource(std::string name, ResourceEntry& ent);

    void updateResourceFromFile(void* data, u32 size);

private:
    std::unordered_map<std::string, ResourceEntry> resources;

    std::unordered_map<std::string, TriangleMesh> meshes;
    std::unordered_map<std::string, Material> materials;

    bool initializeTriangleMesh(TriangleMesh* mesh);
    bool initializeMaterial(Material* mat);
    bool genTextureFromData(Material_Texture* tex);


public:
    static ResourceCatalog* GetInstance();
private:
    static ResourceCatalog* _singleton;
};

#endif
