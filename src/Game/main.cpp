#include "Scene\SceneManager.hpp"
#include "Engine.hpp"
#include "Puppet.hpp"

void GlobalHandleMessage(Message msg);
Engine g_engine;

int main(int argc, char* argv[]) {
    g_engine.Start(GlobalHandleMessage, argc, argv);

    return 0;
}

void GlobalHandleMessage(Message msg) {
    g_engine.globalHandle(msg);
}

bool Scene::recognizeCustomEntity(std::string entType) {
    if (entType.compare("PUPPET") == 0)
        return true;

    return false;
}

void Scene::processCustomEntityLoad(std::string entType, std::istringstream &iss, ResourceManager* resource) {
    if (entType.compare("PUPPET") == 0) {
        Puppet* pup = resource->reserveDataBlocks<Puppet>(1).data;
        pup->parseLevelData(iss, resource);

        m_entities.push_back(pup);
    }

    return;
}