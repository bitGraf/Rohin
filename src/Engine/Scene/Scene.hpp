#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "Resource/ResourceCatalog.hpp"
#include "GLFW/glfw3.h"
#include "Render/RenderBatch.hpp"
#include "Render/Shadowmap.hpp"

#include "EnvironmentMap.hpp"
#include "Utils.hpp"

#include "GameObject/Player.hpp"
#include "GameObject/Camera.hpp"
#include "GameObject/Light.hpp"
#include "GameObject/AICharacter.hpp"
#include "GameObject/TriggerVolume.hpp"
#include "GameObject/Pathfinding.hpp"

const int MAX_GAME_OBJECTS = 100;

class Scene {
public:
    Scene();

    //void CreateFromDataNode(DataNode* root);

    void loadFromFile(std::string path, bool noGLLoad);
    void update(double dt);

    bool recognizeCustomEntity(std::string entType);
    void processCustomEntityLoad(std::string entType, std::istringstream &iss);

    GameObject* getObjectByName(const std::string objectName) const;
    GameObject* getObjectByID(const UID_t id) const;
    UID_t getObjectIDByName(const std::string objectName) const;

//private:
    //DataBlock<GameObject> m_masterList;
    //std::vector<GameObject*> m_masterList;
    std::unordered_map<UID_t, GameObject*> m_masterMap;
    struct GameObjectLists_t {
        void clear() {
            Renderable.clear();
            Cameras.clear();
            PointLights.clear();
            SpotLights.clear();
            DirLights.clear();
            Players.clear();
            Volumes.clear();
			Actor.clear();
        }

        std::vector<RenderableObject*> Renderable;
        std::vector<Camera*> Cameras;
        std::vector<PointLight*> PointLights;
        std::vector<SpotLight*> SpotLights;
        std::vector<DirLight*> DirLights;
        //std::vector<CharacterObject*> Characters;
        std::vector<PlayerObject*> Players;
        std::vector<TriggerVolume*> Volumes;
		std::vector<AICharacter*> Actor;
		std::vector<GoapCharacter*> GoapActor;
    };
    GameObjectLists_t objectsByType;

    EnvironmentMap envMap;

    u16 scr_width, scr_height;

    std::string name;
};

#endif
