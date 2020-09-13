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
    //DataNode root;
    //LoadSceneFromFile(filename, &root);
    //ResourceCatalog::GetInstance()->createNewResource(filename, resLevel, true);

    picojson::value v;
    LoadSceneFromFile(filename, v);

    // Unload current scene in the meantime
    UnloadCurrentScene();

    // Load new data into scene
    //CreateGameObjects(&root);

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

bool SceneManager::LoadSceneFromFile(std::string filename, picojson::value& v) {
    BENCHMARK_FUNCTION();
    FileSystem* fs = FileSystem::GetInstance();

    size_t rawBytesRead = 0;
    char* rawData = fs->readAllBytes(filename, rawBytesRead, true);

    if (!rawBytesRead || !rawData) {
        Console::logError("Failed to open level file '%s'", filename.c_str());
        return false;
    }

    size_t bufSize;
    char* buffer = StripComments(rawData, rawBytesRead, bufSize);

    std::string json(buffer, bufSize);
    std::string err = picojson::parse(v, json);
    free(buffer);
    free(rawData);

    if (!err.empty()) {
        std::cerr << err << std::endl;
        return false;
    }

    picojson::value::object& root = v.get<picojson::object>();
    CreateGameObjects(root);

    return true;
}

void SceneManager::CreateGameObjects(picojson::object& root) {
    BENCHMARK_FUNCTION();

    /* Get major nodes */
    using jo = picojson::object;
    using ja = picojson::array;

    /* Scene Data */
    if (root.find("Scene") != root.end()) {
        jo& sceneNode = root["Scene"].get<jo>();

        currentSceneName = sceneNode["name"].get<std::string>();
    }

    /* Load resources */
    if (root.find("Resources") != root.end()) {
        jo& resourcesNode = root["Resources"].get<jo>();

        ja& files = resourcesNode["files"].get<ja>();
        for (auto it : files) {
            std::string str = it.get<std::string>();

            ResourceCatalog::GetInstance()->loadResourceFile(str);
        }
    }

    /* Load Game Objects */
    if (root.find("GameObjects") != root.end()) {
        jo& gameObjectsNode = root["GameObjects"].get<jo>();


        /* Renderables */
        ja& renderables = gameObjectsNode["Renderables"].get<ja>();
        for (auto it : renderables) {
            jo& ro = it.get<jo>();

            auto k = MemoryPool::GetInstance()->allocBlock<RenderableObject>(1);

            k.data->Create(ro);
            //objectsByType.Renderable.push_back(k.data);
            GameObject* go = k.data;

            if (go) {
                gameObjectList.insert(std::unordered_map<UID_t, GameObject*>::value_type(go->getID(), go));
            }
        }

        /* Camera */
        jo& camObj = gameObjectsNode["Camera"].get<jo>();
        Camera cam;
        cam.Create(camObj);

        /* Player */
        jo& playerObj = gameObjectsNode["Player"].get<jo>();
        PlayerObject play;
        play.Create(camObj);

        /* Lights */
        ja& lights = gameObjectsNode["Lights"].get<ja>();
        for (auto it : lights) {
            jo& ro = it.get<jo>();

            std::string type = safeAccess<std::string>(ro, "type", "point");

            GameObject* go = nullptr;
            if (type.compare("spot") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<SpotLight>(1);

                k.data->Create(ro);

                go = k.data;
                //objectsByType.Renderable.push_back(go);
            } else if (type.compare("dir") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<DirLight>(1);

                k.data->Create(ro);

                go = k.data;
                //objectsByType.Renderable.push_back(go);
            } else if (type.compare("point") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<PointLight>(1);

                k.data->Create(ro);

                go = k.data;
                //objectsByType.Renderable.push_back(go);
            }

            if (go) {
                gameObjectList.insert(std::unordered_map<UID_t, GameObject*>::value_type(go->getID(), go));
            }
        }
    }    

    /* Load Environment */
    if (root.find("Skybox") != root.end()) {
        jo& skybox = root["Skybox"].get<jo>();

        std::string type = safeAccess<std::string>(skybox, "type", "_none_");

        if (type.compare("HDR") == 0) {
            std::string path = skybox["file"].get<std::string>();

            bool noGLLoad = true;
            /* environment map */
            if (!noGLLoad) {
                envMap.loadHDRi(path);
                envMap.preCompute();
            }
        }
    }
}