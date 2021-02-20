#pragma once

#include "Engine/GameObject/Registry.hpp"
#include "Engine/Core/GameMath.hpp"

class SceneHierarchyPanel;

namespace Engine {

    class GameObject;
    class Scene {
    public:
        Scene();
        ~Scene();

        GameObject CreateGameObject(const std::string& name = std::string());

        void loadFromFile(const std::string& filename);
        void writeToFile(const std::string& filename);

        void OnUpdate(double dt);
        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnViewportResize(u32 width, u32 height);

        bool IsPlaying() const { return m_Playing; }

        Registry& GetRegistry() { return m_Registry; }

    private:
        Registry m_Registry;
        u32 m_ViewportWidth = 0, m_ViewportHeight = 0;

        bool m_Playing = false;

        friend class GameObject;
        friend class SceneHierarchyPanel;
    };
}
