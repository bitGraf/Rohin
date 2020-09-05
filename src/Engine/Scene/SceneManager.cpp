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

    // now that all the useful information is collected in one line, parse it into useful blocks
    KVH kvh;
    kvh.CreateAsRoot(nullptr, 0);
    kvh.Destroy();

    printf("New Buffer: (%d)[%s]\n", (int)bufSize, buffer);
    free(buffer);

    return true;
}