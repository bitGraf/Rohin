#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "Message/CoreSystem.hpp"
#include "Entity.hpp"
#include "Resource/ResourceManager.hpp"
#include "Camera.hpp"
#include "Lights.hpp"
#include "GLFW\glfw3.h"
#include "Render\BatchRender.hpp"
#include "Render\Shadowmap.hpp"

#include "EnvironmentMap.hpp"
#include "Utils.hpp"

class Scene {
public:
    Scene();

    void loadFromFile(ResourceManager* resource, std::string path, bool noGLLoad);
    void update(double dt);

    virtual bool recognizeCustomEntity(std::string entType);
    virtual void processCustomEntityLoad(std::string entType, std::istringstream &iss, ResourceManager* resource);

//private:
    std::vector<Entity*> m_entities;
    Camera camera;

    f32 objYaw, camYaw;

    static const int NUM_POINTLIGHTS = 4;
    static const int NUM_SPOTLIGHTS = 4;
    DirLight sun;
    PointLight pointLights[NUM_POINTLIGHTS];
    SpotLight spotLights[NUM_SPOTLIGHTS];

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

#endif
