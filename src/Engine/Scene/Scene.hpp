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

    GameObject* getObjectByName(const std::string objectName) const;
    GameObject* getObjectByID(const UID_t id) const;

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

    EnvironmentMap envMap;

    std::string name;
};

extern Scene* CurrentScene;
Scene* GetScene();
void getRenderBatch(BatchDrawCall* batch);

#endif
