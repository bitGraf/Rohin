#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>

#include "CoreSystem.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"

class Scene {
public:
    Scene();

    void testCreate(ResourceManager* resource);

private:
    std::vector<Entity> m_entities;
};


class SceneManager : public CoreSystem {
public:
    SceneManager();

    // Inherited via CoreSystem
    virtual void update(double dt) override;
    virtual void handleMessage(Message msg) override;
    virtual void destroy() override;
    virtual void sys_create(ConfigurationManager * configMgr) override;

    void loadScenes(ResourceManager* resource);

private:
    Scene* m_currentScene;

    std::vector<Scene> scenes;
};

#endif
