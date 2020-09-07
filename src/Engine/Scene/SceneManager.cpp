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

bool containsChar(char* str, char character) {
    for (int n = 0; n < strlen(str); n++) {
        if (str[n] == character)
            return true;
    }
    return false;
}

bool SceneManager::LoadNewScene(std::string filename) {
    // TODO: make this multithreaded? prob using events
    // queue up a FILE IO operation
    DataNode root;
    std::thread t1(&SceneManager::LoadSceneFromFile, this, filename, &root);
    ResourceCatalog::GetInstance()->createNewResource(filename, resLevel, true);
    //LoadSceneFromFile(filename, &root);

    // Unload current scene in the meantime
    for (auto go : gameObjectList) {
        UID_t id = go.first;
        GameObject* obj = go.second;

        obj->Destroy();
    }
    gameObjectList.clear(); // just remove eveything

    // wait for fileIO to end
    t1.join();

    // Load new data into scene
    CreateGameObjects(&root);

    return true;
}

bool SceneManager::LoadSceneFromFile(std::string filename, DataNode* root) {
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

        /*
        go->Name = node.getData("name").asString();

        float posx = node.getData("posx").asFloat();
        float posy = node.getData("posy").asFloat();
        float posz = node.getData("posz").asFloat();
        go.Position = math::vec3(posx, posy, posz);

        float yaw = node.getData("yaw").asFloat();
        float pitch = node.getData("pitch").asFloat();
        float roll = node.getData("roll").asFloat();
        go.YawPitchRoll = math::vec3(yaw, pitch, roll);

        float scalex = node.getData("scalex").asFloat();
        float scaley = node.getData("scaley").asFloat();
        float scalez = node.getData("scalez").asFloat();
        //go.scale= math::vec3(posx, posy, posz);

        std::string parent = node.getData("parent").asString();
        //go.setParent();

        std::string mesh = node.getData("mesh").asString();
        //go.setMesh(mesh)

        std::string material = node.getData("material").asString();
        //go.setMaterial(material)

        float mesh_posx = node.getData("mesh_posx").asFloat();
        float mesh_posy = node.getData("mesh_posy").asFloat();
        float mesh_posz = node.getData("mesh_posz").asFloat();
        //go.mesh_pos = vec3();

        float mesh_yaw = node.getData("mesh_yaw").asFloat();
        float mesh_pitch = node.getData("mesh_pitch").asFloat();
        float mesh_roll = node.getData("mesh_roll").asFloat();
        //go.mesh_ypr = vec3();

        float mesh_scalex = node.getData("mesh_scalex").asFloat();
        float mesh_scaley = node.getData("mesh_scaley").asFloat();
        float mesh_scalez = node.getData("mesh_scalez").asFloat();
        //go.mesh_scale = vec3();

        float mesh_cullradius = node.getData("mesh_cullRadius").asFloat();
        //go.setCull(mesh_cullradius);
        */

        /*
        auto k = MemoryPool::GetInstance()->allocBlock<RenderableObject>(1);

        k.data->Create(iss);
        objectsByType.Renderable.push_back(k.data);

        go = k.data;

        if (go) {
        // Insert the created GameObject into the hash map, keyed by its unique ID
        m_masterMap.insert(std::unordered_map<UID_t, GameObject*>::value_type(go->getID(), go));
        }
        */
    }
}