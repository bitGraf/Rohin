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

#include "EnvironmentMap.hpp"

class Scene {
public:
    Scene();

    void loadFromFile(ResourceManager* resource, std::string path);
    void update(double dt);

//private:
    std::vector<Entity> m_entities;
    std::vector<Pickable*> m_picks;
    Camera camera;
    u8 cameraMode;

    f32 objYaw, camYaw;

    static const int NUM_POINTLIGHTS = 4;
    static const int NUM_SPOTLIGHTS = 4;
    DirLight sun;
    PointLight pointLights[NUM_POINTLIGHTS];
    SpotLight spotLights[NUM_SPOTLIGHTS];

    GLuint* gridVAO;
    GLuint* numVerts;

    EnvironmentMap envMap;

    std::string name;

    std::string getNextString(std::istringstream& iss);
    math::scalar getNextFloat(std::istringstream& iss);
    math::vec2 getNextVec2(std::istringstream& iss);
    math::vec3 getNextVec3(std::istringstream& iss);
    math::vec4 getNextVec4(std::istringstream& iss);
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
