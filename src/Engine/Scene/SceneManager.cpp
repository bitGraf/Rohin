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

            //m_currentScene->loadFromFile(&g_ResourceManager, "", false);
        }

        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            Console::logMessage("Camera Toggle");

            m_currentScene->cameraMode = m_currentScene->cameraMode == 0 ? 1 : 0;
        }

        if (key == GLFW_KEY_R && action == GLFW_PRESS) {
            Console::logMessage("Camera Reset");

            m_currentScene->camera.yaw = 0;
            m_currentScene->camera.pitch = 0;
            m_currentScene->camera.roll = 0;
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
    Camera::init();

    return this;
}

Scene::Scene() {
    camYaw = 0;
    objYaw = 0;
    cameraMode = 1;
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

    m_entities.clear();

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

            if (entType.compare("DEFAULT") == 0) {
                // Create entity
                Entity ent;
                ent.parseLevelData(iss, resource);

                m_entities.push_back(ent);
                recognizeCustomEntity(entType);
            }
            else if (recognizeCustomEntity(entType)) {
                processCustomEntityLoad(entType, iss);
            }
            else {
                Console::logMessage("Don't recognize entity type: [" 
                    + entType + "]");
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
                spotLights[numSpotLightsLoaded].inner_cutoff = cos(cutoff.x * d2r);
                spotLights[numSpotLightsLoaded].outer_cutoff = cos(cutoff.y * d2r);
                numSpotLightsLoaded++;
            }
        }
        else if (type.compare("CAMERA") == 0) {
            //std::cout << "  parsing volume\n";
            vec3 pos = getNextVec3(iss);
            vec3 rpy = getNextVec3(iss);

            camera.position = pos;
            camera.roll = rpy.x;
            camera.pitch = rpy.y;
            camera.yaw = rpy.z;
        }
    }
    infile.close();
}


void Scene::update(double dt) {
    objYaw += 12 * dt;
    m_entities[0].orientation.toYawPitchRoll(objYaw - 90, 0, 0);

    camera.playerControlled = cameraMode == 1 ? 0 : 1;
    camera.update(dt);
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
        // Set Camera
        m_currentScene->camera.updateViewFrustum(800, 600);
        batch->cameraViewProjectionMatrix = m_currentScene->camera.projectionMatrix *
            m_currentScene->camera.viewMatrix;
        batch->cameraView = m_currentScene->camera.viewMatrix;
        batch->cameraProjection = m_currentScene->camera.projectionMatrix;
        batch->camPos = m_currentScene->camera.position;
        batch->cameraModelMatrix = (
            mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), 
                vec4(m_currentScene->camera.position, 1)) *
                mat4(createYawPitchRollMatrix(m_currentScene->camera.yaw, m_currentScene->camera.pitch, m_currentScene->camera.roll)) *
                mat4(.5, .5, .5, 1));

        mat4 lightView;
        lightView.lookAt(-m_currentScene->sun.direction.get_unit() * 50, vec3(), vec3(0, 1, 0));

        batch->sunViewProjectionMatrix =
            Shadowmap::lightProjection *
            lightView;

        // Set lights
        batch->sun = &m_currentScene->sun;
        batch->pointLights = &m_currentScene->pointLights;
        batch->spotLights = &m_currentScene->spotLights;

        // Environment
        batch->env = &m_currentScene->envMap;

        batch->numCalls = 0;
        // pull every entity
        for (int n = 0; n < m_currentScene->m_entities.size(); n++) {
            if (batch->numCalls >= MAX_CALLS)
                break;
            auto ent = &m_currentScene->m_entities[n];

            if (m_currentScene->camera.withinFrustum(ent->position, 0.25)) { // Check to see if the entity is in the camera frustum
                mat4 modelMatrix = (
                    mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(ent->position, 1)) *
                    mat4(ent->orientation) *
                    mat4(ent->scale.x, ent->scale.y, ent->scale.z, 1));

                batch->calls[batch->numCalls].modelMatrix = modelMatrix;
                batch->calls[batch->numCalls].numVerts = ent->m_mesh->numFaces * 3;
                batch->calls[batch->numCalls].VAO = ent->m_mesh->VAO;
                batch->calls[batch->numCalls].mat = ent->m_material;
                batch->numCalls++;
            }
        }
    }
}