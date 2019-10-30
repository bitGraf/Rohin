#include "SceneManager.hpp"

SceneManager::SceneManager() {
    m_currentScene = nullptr;
}

void SceneManager::update(double dt) {

}

void SceneManager::handleMessage(Message msg) {

}

void SceneManager::destroy() {

}

void SceneManager::sys_create(ConfigurationManager * configMgr) {
}

Scene::Scene() {

}

void Scene::testCreate(ResourceManager* resource) {
    using namespace math;

    Entity ent1;
    ent1.position = vec3(1, 2, 3);
    ent1.orientation = mat3(
        vec3(1, 0, 0),
        vec3(0, 1, 0),
        vec3(0, 0, 1));
    ent1.scale = vec3(1);

    meshRef meshR = resource->getMesh("mesh_cube");
    materialRef matR = resource->getMaterial("material_brick");

    if (meshR == nullptr) {
        printf("Mesh [%s] not loaded.\n", "mesh_cube");
    }
    if (matR == nullptr) {
        printf("Material [%s] not loaded.\n", "material_brick");
    }
    ent1.setMesh(meshR);
    ent1.setMaterial(matR);



    Entity ent2;
    ent2.position = vec3(3, 2, 1);
    ent2.orientation = mat3(
        vec3(1, 0, 0),
        vec3(0, 1, 0),
        vec3(0, 0, 1));
    ent2.scale = vec3(1);

    meshR = resource->getMesh("mesh_cube");
    matR = resource->getMaterial("material_marble");

    if (meshR == nullptr) {
        printf("Mesh [%s] not loaded.\n", "mesh_cube");
    }
    if (matR == nullptr) {
        printf("Material [%s] not loaded.\n", "material_marble");
    }

    ent2.setMesh(meshR);
    ent2.setMaterial(matR);


    m_entities.push_back(ent1);
    m_entities.push_back(ent2);
}

void SceneManager::loadScenes(ResourceManager* resource) {
    Scene s;

    s.testCreate(resource);

    scenes.push_back(s);

    // TODO: Not safe. Pointers change when vector grows
    m_currentScene = &scenes[0];
}