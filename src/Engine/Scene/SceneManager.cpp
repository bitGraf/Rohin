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
    BENCHMARK_FUNCTION();
    return true;
}

void SceneManager::Destroy() {
    BENCHMARK_FUNCTION();
    if (_singleton) {
        delete _singleton;
        _singleton = 0;
    }
}


void SceneManager::Update(double dt) {

}

void SceneManager::GetRenderBatch() {

}

bool containsChar(char* str, char character) {
    for (int n = 0; n < strlen(str); n++) {
        if (str[n] == character)
            return true;
    }
    return false;
}

bool SceneManager::LoadNewScene(std::string filename) {
    BENCHMARK_FUNCTION();
    // TODO: make this multithreaded? prob using events
    // queue up a FILE IO operation
    DataNode root;
    std::thread t1(&SceneManager::LoadSceneFromFile, this, filename, &root);
    ResourceCatalog::GetInstance()->createNewResource(filename, resLevel, true);
    //LoadSceneFromFile(filename, &root);

    // Unload current scene in the meantime
    UnloadCurrentScene();

    // wait for fileIO to end
    t1.join();

    // Load new data into scene
    CreateGameObjects(&root);

    return true;
}

void SceneManager::UnloadCurrentScene() {
    BENCHMARK_FUNCTION();

    for (auto go : gameObjectList) {
        UID_t id = go.first;
        GameObject* obj = go.second;
        int k = 0;

        obj->Destroy();
    }
    gameObjectList.clear(); // just remove eveything
}

bool SceneManager::LoadSceneFromFile(std::string filename, DataNode* root) {
    BENCHMARK_FUNCTION();
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

    //printf("New Buffer: (%d)[%s]\n", (int)bufSize, buffer);
    // now that all the useful information is collected in one line
    // parse it into a useful form
    root->CreateAsRoot(buffer, bufSize);
    free(buffer);

    return true;
}

void SceneManager::CreateGameObjects(DataNode* root) {
    BENCHMARK_FUNCTION();
    std::string name = root->getDataFromPath("Scene.name").asString();
    int numRenderable = root->getDataFromPath("Scene.numRenderable").asInt();

    for (int n = 0; n < numRenderable; n++) {
        std::string path = "Renderable[" + std::to_string(n) + "]";

        DataNode node = root->getChild(path);

        GameObject* go = nullptr;
        auto k = MemoryPool::GetInstance()->allocBlock<RenderableObject>(1);

        k.data->Create(&node);
        //objectsByType.Renderable.push_back(k.data);
        go = k.data;

        if (go) {
            gameObjectList.insert(std::unordered_map<UID_t, GameObject*>::value_type(go->getID(), go));
        }
    }
}