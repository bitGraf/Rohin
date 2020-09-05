#include "Scene.hpp"

Scene::Scene() {
    scr_width = 800;
    scr_height = 600;
}


void Scene::loadFromFile(std::string path, bool noGLLoad) {
    std::ifstream infile(path);

    u32 numSpotLightsLoaded = 0;
    u32 numPointLightsLoaded = 0;

    //m_masterList = resource->reserveDataBlocks<GameObject>(MAX_GAME_OBJECTS);
    m_masterMap.clear();
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
		else if (type.compare("NAVMAP") == 0){
			std::cout << "Doing this";
			//auto k = resource->reserveDataBlocks<PathfindingMap>(1);
			//std::string navMap = getNextString(iss);
			//k.data->loadPfMap(resource, navMap);
		}

        else if (type.compare("SETTING") == 0) {
            std::string settingName = getNextString(iss);
            std::string settingValue = getNextString(iss);

            // operate on setting

        } 
        else if (type.compare("ENTITY") == 0) {      
            std::string entType;
            iss >> entType;

            GameObject* go = nullptr;

            if (entType.compare("GAMEOBJECT") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<GameObject>(1);
                
                k.data->Create(iss);

                go = k.data;
            }
            else if (entType.compare("CAMERA") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<Camera>(1);

                k.data->Create(iss);
                objectsByType.Cameras.push_back(k.data);

                go = k.data;
            }
            else if (entType.compare("RENDERABLE") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<RenderableObject>(1);

                k.data->Create(iss);
                objectsByType.Renderable.push_back(k.data);
                
                go = k.data;
            }
            else if (entType.compare("DIR") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<DirLight>(1);

                k.data->Create(iss);
                objectsByType.DirLights.push_back(k.data);

                go = k.data;
            }
            else if (entType.compare("SPOT") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<SpotLight>(1);

                k.data->Create(iss);
                objectsByType.SpotLights.push_back(k.data);

                go = k.data;
            }
            else if (entType.compare("POINT") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<PointLight>(1);

                k.data->Create(iss);
                objectsByType.PointLights.push_back(k.data);

                go = k.data;
            }
            else if (entType.compare("PLAYER") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<PlayerObject>(1);

                k.data->Create(iss);
                objectsByType.Players.push_back(k.data);
                objectsByType.Renderable.push_back(k.data);
                go = k.data;
            }
            else if (entType.compare("AICHARACTER") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<AICharacter>(1);

                k.data->Create(iss);
                objectsByType.Renderable.push_back(k.data);
				objectsByType.Actor.push_back(k.data);

                go = k.data;
            }
			else if (entType.compare("GOAPCHARACTER") == 0) {
				auto k = MemoryPool::GetInstance()->allocBlock<GoapCharacter>(1);

				k.data->Create(iss);
				objectsByType.Renderable.push_back(k.data);
				objectsByType.GoapActor.push_back(k.data);

				go = k.data;
			}
            else if (entType.compare("VOLUME") == 0) {
                auto k = MemoryPool::GetInstance()->allocBlock<TriggerVolume>(1);

                k.data->Create(iss);
                objectsByType.Volumes.push_back(k.data);

                go = k.data;
            }
            else if (recognizeCustomEntity(entType)) {
                processCustomEntityLoad(entType, iss);
            } 
            else {
                Console::logMessage("Don't recognize entity type: [" 
                    + entType + "]");
            }

            if (go) {
                // Insert the created GameObject into the hash map, keyed by its unique ID
                m_masterMap.insert(std::unordered_map<UID_t, GameObject*>::value_type(go->getID(), go));
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

    /* Run Post-Load functions now that everything is loaded */
    for (auto k : m_masterMap) {
        k.second->PostLoad();
    }
}

void Scene::update(double dt) {
    // TODO : Find better way than iterating through a hash map every frame
    for (auto k : m_masterMap) {
        k.second->Update(dt);
    }
}

GameObject* Scene::getObjectByName(const std::string objectName) const {
    for (auto k : m_masterMap) {
        if (k.second->Name.compare(objectName) == 0) {
            return k.second;
        }
    }
}

GameObject* Scene::getObjectByID(const UID_t id) const {
    if (id) {
        auto it = m_masterMap.find(id);
        if (it != m_masterMap.end()) {
            return it->second;
        }
        else {
            return nullptr;
        }
    }
    else {
        return nullptr;
    }
}

UID_t Scene::getObjectIDByName(const std::string objectName) const {
    for (auto k : m_masterMap) {
        if (k.second->Name.compare(objectName) == 0) {
            return k.second->getID();
        }
    }
    return 0; // Object not found with ObjectName
}


#ifndef CUSTOM_ENTITIES

bool Scene::recognizeCustomEntity(std::string entType) {
    //Console::logMessage("No entities defined");
    return false;
}
void Scene::processCustomEntityLoad(std::string entType, std::istringstream &iss) {

}

#endif



void getRenderBatch(RenderBatch* batch, bool useCull) {
    if (batch == nullptr)
        return;

    /*
    if (CurrentScene) {
        // Set Camera
        auto camera = CurrentScene->objectsByType.Cameras[0];
        camera->updateViewFrustum(CurrentScene->scr_width, CurrentScene->scr_height);

        batch->cameraView = camera->viewMatrix;
        batch->cameraProjection = camera->projectionMatrix;
        //batch->cameraViewProjectionMatrix = batch->cameraProjection * batch->cameraView;

        batch->camPos = camera->Position;
        batch->debugView = camera->viewMatrix;
        batch->cameraModelMatrix = (
            mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0),
                vec4(camera->Position, 1)) *
            mat4(createYawPitchRollMatrix(camera->YawPitchRoll.x, camera->YawPitchRoll.y, camera->YawPitchRoll.z)) *
            mat4(.5, .5, .5, 1));

        mat4 lightView;
        lightView.lookAt(CurrentScene->objectsByType.DirLights[0]->Position, CurrentScene->objectsByType.DirLights[0]->Position + CurrentScene->objectsByType.DirLights[0]->Direction, vec3(0, 1, 0));

        batch->sunViewProjectionMatrix =
            Shadowmap::lightProjection *
            lightView;

        // Set lights
        batch->sun = CurrentScene->objectsByType.DirLights[0];
        int numPoints = min(4, (int)CurrentScene->objectsByType.PointLights.size());
        int numSpots = min(10, (int)CurrentScene->objectsByType.SpotLights.size());

        for (int n = 0; n < 4; n++) {
            if (n < numPoints)
                batch->pointLights[n] = CurrentScene->objectsByType.PointLights[n];
            else
                batch->pointLights[n] = nullptr;
        }
        for (int n = 0; n < 10; n++) {
            if (n < numSpots)
                batch->spotLights[n] = CurrentScene->objectsByType.SpotLights[n];
            else
                batch->spotLights[n] = nullptr;
        }

        // Environment
        batch->env = &CurrentScene->envMap;

        batch->numCalls = 0;
        // pull every renderable object
        for (int n = 0; n < CurrentScene->objectsByType.Renderable.size(); n++) {
            if (batch->numCalls >= MAX_CALLS)
                break;
            auto ent = CurrentScene->objectsByType.Renderable[n];

            if (ent->getMesh()) {
                if (!useCull || ent->noCull || camera->withinFrustum(ent->Position, 0.25)) { // Check to see if the entity is in the camera frustum
                    mat4 modelMatrix = ent->getModelTransform();

                    batch->calls[batch->numCalls].id = ent->getID();
                    batch->calls[batch->numCalls].modelMatrix = modelMatrix;
                    batch->calls[batch->numCalls].numVerts = ent->getMesh()->numFaces * 3;
                    batch->calls[batch->numCalls].VAO = ent->getMesh()->VAO;
                    batch->calls[batch->numCalls].mat = ent->getMaterial();
                    batch->numCalls++;
                }
            }
        }
    }
    */
}
