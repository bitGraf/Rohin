#include "Scene\Scene.hpp"
#include "Engine.hpp"

void GlobalHandleMessage(Message msg);
Engine g_engine;

int main(int argc, char* argv[]) {
    g_engine.InitEngine(GlobalHandleMessage, argc, argv);
    g_engine.LoadLevel("Data/viz.scene");

    g_engine.Start();

    return 0;
}

void GlobalHandleMessage(Message msg) {
    g_engine.globalHandle(msg);
}

bool Scene::recognizeCustomEntity(std::string entType) {
    return false;
}

void Scene::processCustomEntityLoad(std::string entType, std::istringstream &iss, ResourceManager* resource) {
    return;
}