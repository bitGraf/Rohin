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
        EngineLayer(const std::string& name = "EngineLayer") : m_SwapBuffer(false) {}
        EngineLayer(const std::string& name, bool swap) : m_SwapBuffer(swap) {}
        virtual ~EngineLayer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnEvent(Event& event) {}
        virtual void OnGuiRender() {}

        virtual Ref<Scene> GetScene() { return nullptr; }
        virtual Ref<Framebuffer> GetOutput() { return nullptr; }

        virtual void SetSwap(bool swap) { m_SwapBuffer = swap; }
        virtual bool IsSwap() const { return m_SwapBuffer; }

        inline const std::string& GetName() const { return m_name; }

    protected:
        std::string m_name;
        bool m_SwapBuffer;
    };
}