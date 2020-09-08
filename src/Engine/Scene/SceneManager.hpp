#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "Scene.hpp"
#include "Message/EMS.hpp"
#include "Resource/FileSystem.hpp"

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

private:
    bool LoadSceneFromFile(std::string filename, DataNode* root);
    void CreateGameObjects(DataNode* root);
    void UnloadCurrentScene();

    std::unordered_map<UID_t, GameObject*> gameObjectList;

public:
    static SceneManager* GetInstance();
private:
    static SceneManager* _singleton;
};
#endif
