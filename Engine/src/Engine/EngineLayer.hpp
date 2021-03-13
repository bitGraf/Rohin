#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Event/Event.hpp"
#include "Engine/Core/Timing.hpp"

namespace Engine {
    class Scene;
    class Framebuffer;

    class EngineLayer
    {
    public:
        EngineLayer(const std::string& name = "EngineLayer") {}
        virtual ~EngineLayer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnEvent(Event& event) {}
        virtual void OnGuiRender() {}

        inline const std::string& GetName() const { return m_name; }

    protected:
        std::string m_name;
    };
}