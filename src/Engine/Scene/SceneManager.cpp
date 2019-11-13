#include "SceneManager.hpp"

SceneManager::SceneManager() {
    m_currentScene = nullptr;
}

void SceneManager::update(double dt) {
    m_currentScene->yaw += 2 * dt;
    //m_currentScene->m_entities[0].orientation.toYawPitchRoll(m_currentScene->yaw, m_currentScene->yaw, m_currentScene->yaw);
    m_currentScene->m_entities[0].orientation.toYawPitchRoll(m_currentScene->yaw-90, 0, 0);
    //m_currentScene->camera.yaw += 2 * dt;

    if (m_currentScene->yaw > 60) {
        bool k = true;
    }

    f32 yaw = m_currentScene->yaw;

    //yaw = 0;
    m_currentScene->camera.position = vec3(
        4*cos(yaw*d2r),
        sin(yaw*d2r*4)+1.5,
        -4*sin(yaw*d2r)
    );
    m_currentScene->camera.lookAt(vec3(0, 1, 0));
}

void SceneManager::handleMessage(Message msg) {
}

void SceneManager::destroy() {
}

CoreSystem* SceneManager::create() {
    return this;
}

Scene::Scene() {
    yaw = 0;
}

void Scene::testCreate(ResourceManager* resource) {
    using namespace math;

    Entity ent1;
    ent1.position = vec3(0,0,0);
    ent1.orientation.toYawPitchRoll(45, 45, 45);
    ent1.scale = vec3(50);

    meshRef meshR = resource->getMesh("pCube49");
    materialRef matR = resource->getMaterial("Corset_O");

    if (meshR == nullptr) {
        printf("Mesh [%s] not loaded.\n", "Cube");
    }
    if (matR == nullptr) {
        printf("Material [%s] not loaded.\n", "Material");
    }
    ent1.setMesh(meshR);
    ent1.setMaterial(matR);

    m_entities.push_back(ent1);

    gridVAO = &resource->gridVAO;
    numVerts = &resource->numGridVerts;

    /* Setup Lights */
    sun.direction = vec3(-.1, -1, -.1);
    sun.color = vec4(0.412, 0.592, 0.886, 1);
    sun.strength = 10;

    /* Load skybox */
    skybox.loadFromImages("iceflow", ".tga");

    /* environment map */
    envMap.loadHDRi("carpentry_shop_02_4k.hdr");
    envMap.preCompute();
}

void SceneManager::loadScenes(ResourceManager* resource) {
    //Scene s;

    //s.testCreate(resource);
    //scenes.push_back(s);

    Scene sTest;
    sTest.testCreate(resource);
    //sTest.loadFromFile(resource, "");
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
            
            // Create entity
            Entity ent;
            ent.name = entityName;
            ent.setMesh(resource->getMesh(entityMesh));
            ent.setMaterial(resource->getMaterial(entityMat));
            ent.position = entityPos;
            ent.scale = vec3(50);

            m_entities.push_back(ent);
        } 
        else if (type.compare("SKYBOX") == 0) {
            //std::cout << "  parsing skybox\n";

            std::string skyboxType;
            if (!(iss >> skyboxType)) { assert(false); break; } // error

            if (skyboxType.compare("STANDARD") == 0) {
                std::string skyboxFilePath = getNextString(iss);

                /* load skybox */

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