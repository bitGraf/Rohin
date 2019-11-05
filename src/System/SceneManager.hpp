#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>

#include "CoreSystem.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"
#include "Render/Camera.hpp"
#include "Render/Lights.hpp"

class Scene {
public:
    Scene();

    void testCreate(ResourceManager* resource);

//private:
    std::vector<Entity> m_entities;
    Camera camera;

    f32 yaw;

    static const int NUM_POINTLIGHTS = 4;
    static const int NUM_SPOTLIGHTS = 4;
    DirLight sun;
    PointLight pointLights[NUM_POINTLIGHTS];
    SpotLight spotLights[NUM_SPOTLIGHTS];

    GLuint* gridVAO;
    GLuint* numVerts;
};


class SceneManager : public CoreSystem {
public:
    SceneManager();

    // Inherited via CoreSystem
    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    CoreSystem* create();

    void loadScenes(ResourceManager* resource);

    Scene* getCurrentScene();

private:
    Scene* m_currentScene;

    std::vector<Scene> scenes;
};

#endif
