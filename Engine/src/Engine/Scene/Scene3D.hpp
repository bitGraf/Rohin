#pragma once

//#include "Engine/GameObject/Registry.hpp"
#include "Engine/Core/GameMath.hpp"
#include "Engine/Collision/CollisionWorld.hpp"

#include <entt/entity/registry.hpp>

namespace rh {

    class GameObject;
    class Scene3D {
    public:
        Scene3D();
        ~Scene3D();

        GameObject CreateGameObject(const std::string& name = std::string());
        GameObject FindByName(const std::string& name);

        void Destroy();

        bool loadFromLevel(const std::string& filename);

        void OnUpdate(double dt);
        void OnRuntimeStart();
        void OnRuntimePause();
        void OnRuntimeResume();
        void OnRuntimeStop();

        void OnViewportResize(u32 width, u32 height);

        bool IsPlaying() const { return m_Playing; }

        inline void ToggleShowEntityLocations() { ENGINE_LOG_WARN("Showing entity locations: {0}", !m_showEntityLocations); m_showEntityLocations = !m_showEntityLocations; }
        inline void ToggleShowNormals() { ENGINE_LOG_WARN("Showing normals: {0}", !m_showNormals); m_showNormals = !m_showNormals; }
        inline void ToggleCollisionHulls() { ENGINE_LOG_WARN("Showing collision hulls: {0}", !m_showCollisionHulls); m_showCollisionHulls = !m_showCollisionHulls; }

        entt::registry& GetRegistry() { return m_Registry; }
        CollisionWorld& GetCollisionWorld() { return m_cWorld; }

    private:
        entt::registry m_Registry;
        CollisionWorld m_cWorld;
        u32 m_ViewportWidth = 0, m_ViewportHeight = 0;

        bool m_Playing = false;
        bool m_showEntityLocations = false;
        bool m_showCollisionHulls = false;
        bool m_showNormals = false;

        friend class GameObject;
    };

    //extern bool BindGameScript(const std::string& script_tag, Scene3D* scene, GameObject gameobject);
}
