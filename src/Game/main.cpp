#include "Scene\SceneManager.hpp"
#include "Engine.hpp"

Engine g_engine;

void GlobalHandleMessage(Message msg) {
    g_engine.globalHandle(msg);
}

int main(int argc, char* argv[]) {
    g_engine.Start(GlobalHandleMessage, argc, argv);

    return 0;
}

bool Scene::recognizeCustomEntity(std::string entType) {
    return false;
}
void Scene::processCustomEntityLoad(std::string entType, std::istringstream &iss) {
    return;
}