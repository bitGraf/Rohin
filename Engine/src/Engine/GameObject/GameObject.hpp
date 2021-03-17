#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Scene/Scene.hpp"

namespace Engine {
    class GameObject {
    public:
        GameObject()
            : m_GameObjectHandle(0), m_Scene(0) {}
        GameObject(GameObject_type handle, Scene* scene)
            : m_GameObjectHandle(handle), m_Scene(scene) {}
        GameObject(const GameObject& other) = default;

        template<typename Component, typename... Args>
        Component& AddComponent(Args&&... args) {
            ENGINE_LOG_ASSERT(!HasComponent<Component>(), "Entity already has component");

            return m_Scene->m_Registry.emplace<Component>(m_GameObjectHandle, std::forward<Args>(args)...);
        }

        /*template<typename InstanceType>
        void AddNativeScript() {
            auto& scriptComp = AddComponent<NativeScriptComponent>();
            //scriptComp.Bind<InstanceType>(this);
            //return scriptComp;
        }*/

        template<typename Component>
        Component& GetComponent() {
            ENGINE_LOG_ASSERT(HasComponent<Component>(), "Entity does not have component");

            return m_Scene->m_Registry.get<Component>(m_GameObjectHandle);
        }

        template<typename Component>
        bool HasComponent() {
            return m_Scene->m_Registry.has<Component>(m_GameObjectHandle);
        }

        template<typename Component>
        void RemoveComponent() {
            m_Scene->m_Registry.erase<Component>(m_GameObjectHandle);
        }

        operator bool() const { return m_GameObjectHandle != 0; } // TODO: should check if scene is null?

        GameObject_type GetHandle() const { return m_GameObjectHandle; }
        operator uint32_t() const { return (uint32_t)m_GameObjectHandle; }
        bool operator==(const GameObject& other) const { return (m_GameObjectHandle == other.m_GameObjectHandle) && (m_Scene == other.m_Scene);}
        bool operator!=(const GameObject& other) const { return ~operator==(other); }

        const Scene& GetScene() const { return *m_Scene; }

    private:
        GameObject_type m_GameObjectHandle{ 0 };
        Scene* m_Scene = nullptr;
    };
}