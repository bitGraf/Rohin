#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "Scene.hpp"
#include "Message/EMS.hpp"
#include "Resource/FileSystem.hpp"

#include "picojson.h"

#include <thread>

class SceneManager : public MessageReceiver {
public:
    SceneManager();
    ~SceneManager();
    
    bool Init();
    void Destroy();
    
    void Update(double dt);
    void GetRenderBatch();

    bool LoadNewScene(std::string filename);

    std::string currentSceneName;

private:
    bool LoadSceneFromFile(std::string filename, picojson::value& v);
    void CreateGameObjects(picojson::object& root);
    void UnloadCurrentScene();

    std::unordered_map<UID_t, GameObject*> gameObjectList;

public:
    static SceneManager* GetInstance();
private:
    static SceneManager* _singleton;
};
#endif
