#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "Message/CoreSystem.hpp"
#include "Resource/ResourceManager.hpp"
#include "GLFW\glfw3.h"
#include "Render\BatchRender.hpp"
#include "Render\Shadowmap.hpp"

#include "EnvironmentMap.hpp"
#include "Utils.hpp"

#include "GameObject\Player.hpp"
#include "GameObject\Camera.hpp"
#include "GameObject\Light.hpp"

const int MAX_GAME_OBJECTS = 100;

class Scene {
public:
    Scene();

    void loadFromFile(ResourceManager* resource, std::string path, bool noGLLoad);
    void update(double dt);

    virtual bool recognizeCustomEntity(std::string entType);
    virtual void processCustomEntityLoad(std::string entType, std::istringstream &iss, ResourceManager* resource);

//private:
    //DataBlock<GameObject> m_masterList;
    std::vector<GameObject*> m_masterList;
    struct GameObjectLists_t {
        void clear() {
            Renderable.clear();
            Camera.clear();
            PointLights.clear();
            SpotLights.clear();
            DirLights.clear();
        }

        std::vector<RenderableObject*> Renderable;
        std::vector<Camera*> Camera;
        std::vector<PointLight*> PointLights;
        std::vector<SpotLight*> SpotLights;
        std::vector<DirLight*> DirLights;
        //std::vector<CharacterObject*> Characters;
        std::vector<PlayerObject*> Players;
    };
    GameObjectLists_t objectsByType;

    //static const int NUM_POINTLIGHTS = 4;
    //static const int NUM_SPOTLIGHTS = 4;
    //DirLight sun;
    //PointLight pointLights[NUM_POINTLIGHTS];
    //SpotLight spotLights[NUM_SPOTLIGHTS];

    EnvironmentMap envMap;

    std::string name;
};


class SceneManager : public CoreSystem {
public:
    SceneManager();

    // Inherited via CoreSystem
    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    CoreSystem* create();

    void loadScenes(ResourceManager* resource, bool testing = false);
    void getRenderBatch(BatchDrawCall* batch);

    Scene* getCurrentScene();

private:
    Scene* m_currentScene;

    std::vector<Scene> scenes;
};

extern Scene* CurrentScene;
Scene* GetScene();

#endif
