#pragma once

#include "Engine/GameObject/GameObject.hpp"

namespace Engine {
    class ScriptableBase {
    public:
        virtual ~ScriptableBase() {}
        
        template<typename Component>
        Component& GetComponent() {
            return m_GameObject.GetComponent<Component>();
        }

        GameObject_type GetGameObjectID() const { return m_GameObject.GetHandle(); }

        const Scene& GetScene() const { return m_GameObject.GetScene(); }

    protected:
        // Interface functions
        virtual void OnCreate() {};
        virtual void OnDestroy() {};
        virtual void OnUpdate(double ts) {};

    private:
        GameObject m_GameObject; // the GameObject its attached to
        friend class Scene;
    };
}