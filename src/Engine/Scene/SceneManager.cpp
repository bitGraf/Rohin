#include "SceneManager.hpp"

SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}

SceneManager* SceneManager::_singleton = 0;

SceneManager* SceneManager::GetInstance() {
    if (!_singleton) {
        _singleton = new SceneManager;
    }

    return _singleton;
}

bool SceneManager::Init() {
    return true;
}

void SceneManager::Destroy() {
    if (_singleton) {
        delete _singleton;
        _singleton = 0;
    }
}


void SceneManager::Update(double dt) {

}

void SceneManager::GetRenderBatch() {

}

void SceneManager::ChangeScene(u32 newScene) {
    if (newScene >= 0 && newScene < sceneList.size()) {
        // unload current scene
        // change scenes
        // load new scene
    }
}

bool containsChar(char* str, char character) {
    for (int n = 0; n < strlen(str); n++) {
        if (str[n] == character)
            return true;
    }
    return false;
}

bool SceneManager::AddSceneToList(std::string filename) {
    FileSystem* fs = FileSystem::GetInstance();

    size_t bytesRead = 0;
    char* data = fs->readAllBytes(filename, bytesRead, true);

    if (!bytesRead || !data) {
        Console::logError("Failed to open level file '%s'", filename.c_str());
        return false;
    }

    size_t bufSize = 0;
    char* buffer = StripComments(data, bytesRead, bufSize);
    free(data);

    printf("New Buffer: (%d)[%s]\n", (int)bufSize, buffer);
    // now that all the useful information is collected in one line
    // parse it into a useful form
    DataNode root;
    root.CreateAsRoot(buffer, bufSize);

    free(buffer);

    // free to use root for all scene needs
    Console::logMessage("Root Node:");
    Console::logMessage(" Data: %d", root.data.size());
    Console::logMessage(" Children: %d", root.children.size());

    Console::logMessage(" Scene Node:");
    Console::logMessage("  Data: %d",       root.getChild("Scene").data.size());
    Console::logMessage("   name: %s",      root.getChild("Scene").getData("name").asString().c_str());
    Console::logMessage("   gravity: %.2f", root.getChild("Scene").getData("gravity").asFloat());
    Console::logMessage("   raining: %s",   root.getChild("Scene").getData("raining").asBool() ? "true" : "false");
    Console::logMessage("  Children: %d",   root.getChild("Scene").children.size());

    Console::logMessage(" GameObject Node:");
    Console::logMessage("  Data: %d",       root.getChild("GameObject").data.size());
    Console::logMessage("   type: %d",      root.getChild("GameObject").getData("type").asInt());
    Console::logMessage("   name: %s",      root.getChild("GameObject").getData("name").asString().c_str());
    Console::logMessage("  Children: %d",   root.getChild("GameObject").children.size());

    return true;
}