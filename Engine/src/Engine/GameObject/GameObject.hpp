#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Scene/Scene3D.hpp"

namespace rh {
    class GameObject {
    public:
        GameObject()
            : m_GameObjectHandle(entt::null), m_Scene(0) {}
        GameObject(GameObject_type handle, Scene3D* scene)
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

        // TODO: do we need this functionality??
        template<typename Component>
        bool HasComponent() {
            return m_Scene->m_Registry.all_of<Component>(m_GameObjectHandle);
        }

        template<typename Component>
        void RemoveComponent() {
            m_Scene->m_Registry.erase<Component>(m_GameObjectHandle);
        }

        operator bool() const { return m_GameObjectHandle != entt::null; } // TODO: should check if scene is null?

        GameObject_type GetHandle() const { return m_GameObjectHandle; }
        operator uint32_t() const { return (uint32_t)m_GameObjectHandle; }
        bool operator==(const GameObject& other) const { return (m_GameObjectHandle == other.m_GameObjectHandle) && (m_Scene == other.m_Scene);}
        bool operator!=(const GameObject& other) const { return (m_GameObjectHandle != other.m_GameObjectHandle) || (m_Scene != other.m_Scene);}

        const Scene3D& GetScene() const { return *m_Scene; }
        Scene3D& GetScene() { return *m_Scene; }

    private:
        GameObject_type m_GameObjectHandle{ 0 };
        Scene3D* m_Scene = nullptr;
    };
}