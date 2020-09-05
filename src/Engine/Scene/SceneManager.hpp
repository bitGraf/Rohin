#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "Scene.hpp"
#include "Message/EMS.hpp"
#include "Resource/FileSystem.hpp"

class SceneManager : public MessageReceiver {
public:
    SceneManager();
    ~SceneManager();
    
    bool Init();
    void Destroy();
    
    void Update(double dt);
    void GetRenderBatch();

    bool AddSceneToList(std::string filename);
    void ChangeScene(u32 newScene);
    //Scene* GetCurrentScene();

private:
    std::vector<Scene> sceneList;

    u32 currentScene;

public:
    static SceneManager* GetInstance();
private:
    static SceneManager* _singleton;
};
#endif
