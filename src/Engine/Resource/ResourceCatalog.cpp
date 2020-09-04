#include "ResourceCatalog.hpp"

//ResourceCatalog g_ResourceCatalog;

ResourceCatalog::ResourceCatalog()
{
}

ResourceCatalog::~ResourceCatalog()
{
}

ResourceCatalog* ResourceCatalog::_singleton = 0;

ResourceCatalog* ResourceCatalog::GetInstance() {
    if (!_singleton) {
        _singleton = new ResourceCatalog;
    }
    return _singleton;
}

bool ResourceCatalog::Init() {
    Register(eveFileUpdate, this, (Callback)&ResourceCatalog::updateResourceFromFile);

    return true;
}

void ResourceCatalog::Destroy() {
    if (_singleton) {
        delete _singleton;
        _singleton = 0;
    }
}

void ResourceCatalog::createNewResource(char* name, ResourceEntryType type, bool empty) {
    ResourceEntry ent;

    ent.filename = std::string(name);
    ent.data = 0;
    ent.size = 0;
    ent.type = type;

    resources[ent.filename] = ent;
    FileSystem::GetInstance()->watchFile(ent.filename, empty);
}

/*
materialRef ResourceManager::getMaterial(std::string id) {
if (materials.find(id) == materials.end()) {
return nullptr;
}
else {
return &materials[id]; //TODO: This might not be safe.
}
}
*/

bool ResourceCatalog::getResource(std::string name, ResourceEntry& ent) {
    if (resources.find(name) == resources.end()) {
        return false;
    }
    else {
        ent = resources[name];
        return true;
    }
}

void ResourceCatalog::updateResourceFromFile(void* data, u32 size) {
    std::string filename((char*)data, size);

    ResourceEntry ent;
    if (!getResource(filename, ent)) {
        Console::logError("Failed to find resource %s", filename);
    }
    else {
        switch (ent.type) {
        case resMesh:
            Console::logMessage("Updating Mesh Resource from %s", ent.filename.c_str());
            break;
        case resImage:
            Console::logMessage("Updating Image Resource from %s", ent.filename.c_str());
            break;
        case resFont:
            Console::logMessage("Updating Font Resource from %s", ent.filename.c_str());
            break;
        case resShader:
            Console::logMessage("Updating Shader Resource from %s", ent.filename.c_str());
            break;
        case resOther:
            Console::logMessage("Updating Other Resource from %s", ent.filename.c_str());
            break;
        }
    }
}

/*
void Engine::fileUpdateCallback(void* data, u32 size) {
    if (data && size > 0) {
        // use %.*s because data is not a null-terminated string
        Console::logMessage("File '%.*s' Updated!", size, (char*)data);
    }
    else {
        Console::logError("Expected data on event message");
    }
}
*/