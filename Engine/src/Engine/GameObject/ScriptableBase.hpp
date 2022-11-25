#pragma once

#include "Engine/GameObject/GameObject.hpp"

namespace rh {
    class ScriptableBase {
    public:
        virtual ~ScriptableBase() {}
        
        // Helper functions for all native scripts
        template<typename Component>
        Component& GetComponent() { return m_GameObject.GetComponent<Component>(); }

        GameObject_type GetGameObjectID() const { return m_GameObject.GetHandle(); }

        const Scene3D& GetScene() const { return m_GameObject.GetScene(); }

    protected:
        // Interface functions
        virtual void OnCreate() {};
        virtual void OnLink() {};
        virtual void OnDestroy() {};
        virtual void OnUpdate(double ts) {};

        Scene3D& GetScene() { return m_GameObject.GetScene(); }
    private:
        GameObject m_GameObject; // the GameObject its attached to
        friend class Scene3D;
    };
}