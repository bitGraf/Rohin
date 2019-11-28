#include "SceneManager.hpp"

Scene* CurrentScene = nullptr;
Scene* GetScene() { return CurrentScene; }

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

            //m_currentScene->loadFromFile(&g_ResourceManager, "", false);
        }

        if (key == GLFW_KEY_R && action == GLFW_PRESS) {
            Console::logMessage("Camera Reset");
        }

        if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
            Console::logMessage("FPS Limit Toggle");

            if (g_options.limitFramerate) {
                if (g_options.highFramerate) {
                    g_options.limitFramerate = false;
                    g_options.highFramerate = false;
                }
                else {
                    g_options.highFramerate = true;
                }
            }
            else {
                g_options.limitFramerate = true;
            }
        }
    }
}

void SceneManager::destroy() {
}

CoreSystem* SceneManager::create() {
    return this;
}

Scene::Scene() {
}

void SceneManager::loadScenes(ResourceManager* resource, bool testing) {
    //Scene s;

    //s.testCreate(resource);
    //scenes.push_back(s);

    Scene sTest;
    //sTest.testCreate(resource);
    sTest.loadFromFile(resource, "", testing);
    scenes.push_back(sTest);

    // TODO: Not safe. Pointers change when vector grows
    m_currentScene = &scenes[0];
}

Scene* SceneManager::getCurrentScene() {
    return m_currentScene;
}

void Scene::loadFromFile(ResourceManager* resource, std::string path, bool noGLLoad) {
    std::ifstream infile("Data/test.scene");

    u32 numSpotLightsLoaded = 0;
    u32 numPointLightsLoaded = 0;

    //m_masterList = resource->reserveDataBlocks<GameObject>(MAX_GAME_OBJECTS);
    m_masterList.clear();
    objectsByType.clear();

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
            if (!noGLLoad)
                resource->loadModelFromFile(resourceFilename, true);
        } 
        else if (type.compare("ENTITY") == 0) {      
            std::string entType;
            iss >> entType;

            GameObject* go = nullptr;

            if (entType.compare("GAMEOBJECT") == 0) {
                auto k = resource->reserveDataBlocks<GameObject>(1);
                
                k.data->Create(iss, resource);

                go = k.data;
            }
            else if (entType.compare("CAMERA") == 0) {
                auto k = resource->reserveDataBlocks<Camera>(1);

                k.data->Create(iss, resource);
                objectsByType.Camera.push_back(k.data);

                go = k.data;
            }
            else if (entType.compare("RENDERABLE") == 0) {
                auto k = resource->reserveDataBlocks<RenderableObject>(1);

                k.data->Create(iss, resource);
                objectsByType.Renderable.push_back(k.data);
                
                go = k.data;
            }
            else if (entType.compare("DIR") == 0) {
                auto k = resource->reserveDataBlocks<DirLight>(1);

                k.data->Create(iss, resource);
                objectsByType.DirLights.push_back(k.data);

                go = k.data;
            }
            else if (entType.compare("SPOT") == 0) {
                auto k = resource->reserveDataBlocks<SpotLight>(1);

                k.data->Create(iss, resource);
                objectsByType.SpotLights.push_back(k.data);

                go = k.data;
            }
            else if (entType.compare("POINT") == 0) {
                auto k = resource->reserveDataBlocks<PointLight>(1);

                k.data->Create(iss, resource);
                objectsByType.PointLights.push_back(k.data);

                go = k.data;
            }
            else if (entType.compare("PLAYER") == 0) {
                auto k = resource->reserveDataBlocks<PlayerObject>(1);

                k.data->Create(iss, resource);
                objectsByType.Players.push_back(k.data);
                objectsByType.Renderable.push_back(k.data);

                go = k.data;
            }
            else if (recognizeCustomEntity(entType)) {
                processCustomEntityLoad(entType, iss, resource);
            } 
            else {
                Console::logMessage("Don't recognize entity type: [" 
                    + entType + "]");
            }

            if (go) {
                m_masterList.push_back(go);
            }
        } 
        else if (type.compare("SKYBOX") == 0) {
            //std::cout << "  parsing skybox\n";

            std::string skyboxType;
            if (!(iss >> skyboxType)) { assert(false); break; } // error

            if (skyboxType.compare("STANDARD") == 0) {
                std::string skyboxFilePath = getNextString(iss);
                std::string skyboxFileType = getNextString(iss);

                /* load skybox */
                if (!noGLLoad) {
                    envMap.loadSkybox(skyboxFilePath, skyboxFileType);
                    envMap.preCompute();
                }

            } else if (skyboxType.compare("HDR") == 0) {
                std::string hdrFilePath = getNextString(iss);

                /* environment map */
                if (!noGLLoad) {
                    envMap.loadHDRi(hdrFilePath);
                    envMap.preCompute();
                }
            }
        }
    }
    infile.close();
}


void Scene::update(double dt) {
    for (int n = 0; n < m_masterList.size(); n++) {
        m_masterList[n]->Update(dt);
    }
}

#ifndef CUSTOM_ENTITIES

bool Scene::recognizeCustomEntity(std::string entType) {
    Console::logMessage("No entities defined");
    return false;
}
void Scene::processCustomEntityLoad(std::string entType, std::istringstream &iss) {

}

#endif


void SceneManager::getRenderBatch(BatchDrawCall* batch) {
    if (batch == nullptr)
        return;

    if (m_currentScene) {
        batch->currScene = m_currentScene;

        // Set Camera
        auto camera = m_currentScene->objectsByType.Camera[0];
        camera->updateViewFrustum(800, 600);

        batch->cameraView = camera->viewMatrix;
        batch->cameraProjection = camera->projectionMatrix;
        batch->cameraViewProjectionMatrix = batch->cameraProjection * batch->cameraView;

        batch->camPos = camera->Position;
        batch->cameraModelMatrix = (
            mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), 
                vec4(camera->Position, 1)) *
                mat4(createYawPitchRollMatrix(camera->YawPitchRoll.x, camera->YawPitchRoll.y, camera->YawPitchRoll.z)) *
                mat4(.5, .5, .5, 1));

        mat4 lightView;
        lightView.lookAt(m_currentScene->objectsByType.DirLights[0]->Position, vec3(), vec3(0, 1, 0));

        batch->sunViewProjectionMatrix =
            Shadowmap::lightProjection *
            lightView;

        // Set lights
        batch->sun = m_currentScene->objectsByType.DirLights[0];
        int numPoints = min(4, (int)m_currentScene->objectsByType.PointLights.size());
        int numSpots  = min(4, (int)m_currentScene->objectsByType.SpotLights.size());
        
        for (int n = 0; n < 4; n++) {
            if (n < numPoints)
                batch->pointLights[n] = m_currentScene->objectsByType.PointLights[n];
            else
                batch->pointLights[n] = nullptr;
        }
        for (int n = 0; n < 4; n++) {
            if (n < numSpots)
                batch->spotLights[n] = m_currentScene->objectsByType.SpotLights[n];
            else
                batch->spotLights[n] = nullptr;
        }

        // Environment
        batch->env = &m_currentScene->envMap;

        batch->numCalls = 0;
        // pull every entity
        for (int n = 0; n < m_currentScene->objectsByType.Renderable.size(); n++) {
            if (batch->numCalls >= MAX_CALLS)
                break;
            auto ent = m_currentScene->objectsByType.Renderable[n];

            if (camera->withinFrustum(ent->Position, 0.25)) { // Check to see if the entity is in the camera frustum
                mat4 modelMatrix = ent->getModelTransform();

                batch->calls[batch->numCalls].modelMatrix = modelMatrix;
                batch->calls[batch->numCalls].numVerts = ent->getMesh()->numFaces * 3;
                batch->calls[batch->numCalls].VAO = ent->getMesh()->VAO;
                batch->calls[batch->numCalls].mat = ent->getMaterial();
                batch->numCalls++;
            }
        }
    }
}