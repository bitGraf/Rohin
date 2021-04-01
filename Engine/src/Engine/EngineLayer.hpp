#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Event/Event.hpp"
#include "Engine/Core/Timing.hpp"

namespace Engine {
    class Scene3D;
    class Framebuffer;

    class EngineLayer
    {
    public:
        EngineLayer(const std::string& name = "EngineLayer");
        virtual ~EngineLayer();

        virtual void OnAttach() { }
        virtual void OnDetach() { }
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnEvent(Event& event) {}
        virtual void OnGuiRender() {}

        virtual bool IsActive() { return m_LayerActive; }

        inline const std::string& GetName() const { return m_name; }

    protected:
        std::string m_name;
        bool m_LayerActive;

        friend class Application;
    };
}