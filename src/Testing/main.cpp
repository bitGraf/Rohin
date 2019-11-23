#include "Scene\SceneManager.hpp"

#include "Engine.hpp"

int main(int argc, char* argv[]) {
    Engine engine;

    engine.Start();

    return 0;
}

bool Scene::recognizeCustomEntity(std::string entType) {
    Console::logMessage("No entities defined");
    return false;
}
void Scene::processCustomEntityLoad(std::string entType, std::istringstream &iss) {
    return;
}