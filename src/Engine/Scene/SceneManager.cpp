#include "SceneManager.hpp"

SceneManager::SceneManager() {
    m_currentScene = nullptr;
}

void SceneManager::update(double dt) {
    m_currentScene->update(dt);
}

void SceneManager::handleMessage(Message msg) {
    if (msg.isType("InputKey")) {
        // int button, int action, int mods
        using dt = Message::Datatype;
        dt key = msg.data[0];
        dt scancode = msg.data[1];
        dt action = msg.data[2];
        dt mods = msg.data[3];

        if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
            Console::logMessage("Reloading level");

            m_currentScene->loadFromFile(&g_ResourceManager, "");
        }

        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            Console::logMessage("Camera Toggle");

            m_currentScene->cameraMode = m_currentScene->cameraMode == 0 ? 1 : 0;
        }
    }
}

void SceneManager::destroy() {
}

CoreSystem* SceneManager::create() {
    return this;
}

Scene::Scene() {
    camYaw = 0;
    objYaw = 0;
    cameraMode = 1;
}

void SceneManager::loadScenes(ResourceManager* resource) {
    //Scene s;

    //s.testCreate(resource);
    //scenes.push_back(s);

    Scene sTest;
    //sTest.testCreate(resource);
    sTest.loadFromFile(resource, "");
    scenes.push_back(sTest);

    // TODO: Not safe. Pointers change when vector grows
    m_currentScene = &scenes[0];
}

Scene* SceneManager::getCurrentScene() {
    return m_currentScene;
}

void Scene::loadFromFile(ResourceManager* resource, std::string path) {
    std::ifstream infile("Data/test.scene");

    u32 numSpotLightsLoaded = 0;
    u32 numPointLightsLoaded = 0;

    m_entities.clear();
    m_picks.clear();

    std::string line;
    while (std::getline(infile, line))
    {
        if (line.empty()) { continue; }
        std::istringstream iss(line);

        std::string type;
        if (!(iss >> type)) { assert(false); break; } // error

        if (type.compare("SCENE") == 0) {
            std::string sceneName = getNextString(iss);

            // set scene name
            name = sceneName;
        } 
        else if (type.compare("SETTING") == 0) {
            std::string settingName = getNextString(iss);
            std::string settingValue = getNextString(iss);

            // operate on setting

        } 
        else if (type.compare("RESOURCE") == 0) {
            std::string resourceFilename = getNextString(iss);

            // load resource pack
            resource->loadModelFromFile(resourceFilename, true);
        } 
        else if (type.compare("ENTITY") == 0) {
            std::string entityName = getNextString(iss);
            std::string entityMesh = getNextString(iss);
            std::string entityMat  = getNextString(iss);
            math::vec3 entityPos   = getNextVec3(iss);
            scalar entityScale     = getNextFloat(iss);
            
            // Create entity
            Entity ent;
            ent.name = entityName;
            ent.setMesh(resource->getMesh(entityMesh));
            ent.setMaterial(resource->getMaterial(entityMat));
            ent.position = entityPos;
            ent.scale = vec3(entityScale);

            m_entities.push_back(ent);
        } 
        else if (type.compare("SKYBOX") == 0) {
            //std::cout << "  parsing skybox\n";

            std::string skyboxType;
            if (!(iss >> skyboxType)) { assert(false); break; } // error

            if (skyboxType.compare("STANDARD") == 0) {
                std::string skyboxFilePath = getNextString(iss);
                std::string skyboxFileType = getNextString(iss);

                /* load skybox */
                envMap.loadSkybox(skyboxFilePath, skyboxFileType);
                envMap.preCompute();

            } else if (skyboxType.compare("HDR") == 0) {
                std::string hdrFilePath = getNextString(iss);

                /* environment map */
                envMap.loadHDRi(hdrFilePath);
                envMap.preCompute();
            }
        } 
        else if (type.compare("LIGHT") == 0) {
            //std::cout << "  parsing skybox\n";

            std::string lightType;
            if (!(iss >> lightType)) { assert(false); break; } // error

            if (lightType.compare("DIR") == 0) {
                math::scalar strength = getNextFloat(iss);
                math::vec4 color = getNextVec4(iss);
                math::vec3 dir = getNextVec3(iss);

                // create light
                sun.strength = strength;
                sun.color = color;
                sun.direction = dir;

            } else if (lightType.compare("POINT") == 0) {
                math::scalar strength = getNextFloat(iss);
                math::vec4 color = getNextVec4(iss);
                math::vec3 pos = getNextVec3(iss);

                // create light
                if (numPointLightsLoaded == NUM_POINTLIGHTS) continue;

                pointLights[numPointLightsLoaded].strength = strength;
                pointLights[numPointLightsLoaded].color = color;
                pointLights[numPointLightsLoaded].position = pos;
                numPointLightsLoaded++;

            } else if (lightType.compare("SPOT") == 0) {
                math::scalar strength = getNextFloat(iss);
                math::vec4 color = getNextVec4(iss);
                math::vec3 pos = getNextVec3(iss);
                math::vec3 dir = getNextVec3(iss);
                math::vec2 cutoff = getNextVec2(iss);

                // create light
                if (numSpotLightsLoaded == NUM_SPOTLIGHTS) continue;

                spotLights[numSpotLightsLoaded].strength = strength;
                spotLights[numSpotLightsLoaded].color = color;
                spotLights[numSpotLightsLoaded].position = pos;
                spotLights[numSpotLightsLoaded].direction = dir;
                spotLights[numSpotLightsLoaded].inner_cutoff = cutoff.x;
                spotLights[numSpotLightsLoaded].outer_cutoff = cutoff.y;
                numSpotLightsLoaded++;
            }
        }
    }
    infile.close();

    gridVAO = &resource->gridVAO;
    numVerts = &resource->numGridVerts;

    for (int n = 0; n < m_entities.size(); n++) {
        m_picks.push_back(&m_entities[n]);
    }
}

std::string Scene::getNextString(std::istringstream& iss) {
    std::string str;
    std::getline(iss, str, '"');
    std::getline(iss, str, '"');

    return str;
}

math::scalar Scene::getNextFloat(std::istringstream& iss) {
    math::scalar v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v;    
    std::getline(iss, garb, '"');

    return v;
}

math::vec2 Scene::getNextVec2(std::istringstream& iss) {
    math::vec2 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y;
    std::getline(iss, garb, '"');

    return v;
}

math::vec3 Scene::getNextVec3(std::istringstream& iss) {
    math::vec3 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y >> v.z;
    std::getline(iss, garb, '"');

    return v;
}

math::vec4 Scene::getNextVec4(std::istringstream& iss) {
    math::vec4 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y >> v.z >> v.w;
    std::getline(iss, garb, '"');

    return v;
}


void Scene::update(double dt) {
    objYaw += 12 * dt;
    m_entities[0].orientation.toYawPitchRoll(objYaw - 90, 0, 0);

    if (cameraMode) {
        camYaw -= 12 * dt;

        camera.position = vec3(
            2 * cos(camYaw*d2r),
            sin(camYaw*d2r * 4) + 1.5,
            -2 * sin(camYaw*d2r)
        );
        camera.lookAt(vec3(0, 1, 0));
    }
}