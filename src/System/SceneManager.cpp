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
    //m_entities.push_back(ent2);

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

    // TODO: Not safe. Pointers change when vector grows
    m_currentScene = &scenes[0];
}

Scene* SceneManager::getCurrentScene() {
    return m_currentScene;
}