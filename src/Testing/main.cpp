#include "Configuration.hpp"

#include <stdio.h>
#include "Scene\Entity.hpp"
#include "StaticEntity.hpp"
#include "DynamicEntity.hpp"
#include "Scene\SceneManager.hpp"

SceneManager g_SceneManager;

FileSystem g_FileSystem;

int main(int argc, char* argv[]) {
    EnsureDataTypeSize();

    g_SceneManager.create();
    g_ResourceManager.create();
    g_FileSystem.create();

    g_ResourceManager.setFileSystem(&g_FileSystem);

    g_SceneManager.loadScenes(&g_ResourceManager, true); // Load dummy scene to test

    g_SceneManager.destroy();
    g_ResourceManager.destroy();

    system("pause");

    return 0;
}

bool Scene::recognizeCustomEntity(std::string entType) {
    if (entType.compare("STATIC") == 0) {
        std::cout << "Static entity: ";
        return true;
    }
    else if (entType.compare("DYNAMIC") == 0) {
        std::cout << "Dynamic entity: ";
        return true;
    }

    return false;
}

void Scene::processCustomEntityLoad(std::string entType, std::istringstream &iss) {
    if (entType.compare("STATIC") == 0) {
        StaticEntity ent;
        ent.parseLevelData(iss);
        m_entities.push_back(ent);
    }
    else if (entType.compare("DYNAMIC") == 0) {
        DynamicEntity ent;
        ent.parseLevelData(iss);
        m_entities.push_back(ent);
    }
}