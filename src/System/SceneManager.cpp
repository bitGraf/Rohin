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
    sun.direction = vec3(-.1, -.1, -1);
    sun.color = vec4(0.412, 0.592, 0.886, 1);
    sun.strength = 0;

    /* Load skybox */
    skybox.loadFromImages("iceflow", ".tga");

    /* environment map */
    envMap.loadHDRi("carpentry_shop_02_4k.hdr");
    envMap.preCompute();
}

void SceneManager::loadScenes(ResourceManager* resource) {
    Scene s;

    s.testCreate(resource);
    scenes.push_back(s);

    Scene sTest;
    sTest.loadFromFile(resource, "");
    scenes.push_back(sTest);

    // TODO: Not safe. Pointers change when vector grows
    m_currentScene = &scenes[1];
}

Scene* SceneManager::getCurrentScene() {
    return m_currentScene;
}

void Scene::loadFromFile(ResourceManager* resource, std::string path) {
    std::ifstream infile("Data/test.scene");

    std::string line;
    while (std::getline(infile, line))
    {
        if (line.empty()) { continue; }
        //std::cout << "Line [" << line << "]" << std::endl;
        std::istringstream iss(line);
        
        std::string type;
        if (!(iss >> type)) { assert(false); break; } // error

        //std::cout << "  Type: [" << type << "]" << std::endl;
        if (type.compare("SCENE") == 0) {
            //std::cout << "    parsing scene\n";

            std::string sceneName;
            std::getline(iss, sceneName, '"');
            std::getline(iss, sceneName, '"');

            std::cout << "  sceneName: [" << sceneName << "]\n";

            name = sceneName;

        } else if (type.compare("SETTING") == 0) {
            //std::cout << "    parsing setting\n";

            std::string settingName;
            std::getline(iss, settingName, '"');
            std::getline(iss, settingName, '"');

            std::cout << "  setting: [" << settingName << "]";

            std::string settingValue;
            std::getline(iss, settingValue, '"');
            std::getline(iss, settingValue, '"');

            std::cout << ":[" << settingValue << "]\n";

        } else if (type.compare("RESOURCE") == 0) {
            //std::cout << "    parsing resource\n";
            std::string resourceFilename;
            std::getline(iss, resourceFilename, '"');
            std::getline(iss, resourceFilename, '"');

            std::cout << "  resourceFile: [" << resourceFilename << "]\n";

            resource->loadModelFromFile(resourceFilename, true);
        } else if (type.compare("ENTITY") == 0) {
            //std::cout << "    parsing entity\n";

            std::string entityName;
            std::getline(iss, entityName, '"');
            std::getline(iss, entityName, '"');

            std::cout << "  entity: [" << entityName << "]";

            std::string entityMesh;
            std::getline(iss, entityMesh, '"');
            std::getline(iss, entityMesh, '"');

            std::cout << ":[" << entityMesh << "]";

            std::string entityMat;
            std::getline(iss, entityMat, '"');
            std::getline(iss, entityMat, '"');

            std::cout << ":[" << entityMat << "]";

            std::string entityPosition;
            std::getline(iss, entityPosition, '"');
            
            math::vec3 v;
            iss >> v.x >> v.y >> v.z;
            std::cout << ":" << v << std::endl;

            Entity ent;
            ent.name = entityName;
            ent.setMesh(resource->getMesh(entityMesh));
            ent.setMaterial(resource->getMaterial(entityMat));
            ent.position = v;
            ent.scale = vec3(50);

            m_entities.push_back(ent);

        } else if (type.compare("SKYBOX") == 0) {
            //std::cout << "  parsing skybox\n";

            std::string skyboxType;
            if (!(iss >> skyboxType)) { assert(false); break; } // error

            if (skyboxType.compare("STANDARD") == 0) {
                std::string skyboxFilePath;
                std::getline(iss, skyboxFilePath, '"');
                std::getline(iss, skyboxFilePath, '"');

                std::cout << "  filepath Skybox: [" << skyboxFilePath << "]";
            } else if (skyboxType.compare("HDR") == 0) {
                std::string hdrFilePath;
                std::getline(iss, hdrFilePath, '"');
                std::getline(iss, hdrFilePath, '"');

                std::cout << "  filepath HDR: [" << hdrFilePath << "]";

                /* environment map */
                envMap.loadHDRi(hdrFilePath);
                envMap.preCompute();
            }
        }
    }
    infile.close();

    gridVAO = &resource->gridVAO;
    numVerts = &resource->numGridVerts;
}