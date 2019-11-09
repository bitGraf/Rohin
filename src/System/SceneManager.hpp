#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "CoreSystem.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"
#include "Render/Camera.hpp"
#include "Render/Lights.hpp"
#include "Render/SkyBox.hpp"

#include "Render/EnvironmentMap.hpp"

class Scene {
public:
    Scene();

    void testCreate(ResourceManager* resource);

    void loadFromFile(ResourceManager* resource, std::string path);

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

    SkyBox skybox;
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
