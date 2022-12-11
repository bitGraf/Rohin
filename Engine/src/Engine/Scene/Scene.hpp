#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Event/Event.hpp"
#include "Engine/Core/Timing.hpp"

namespace rh {

    class Scene {
    public:
        virtual ~Scene() = default;
        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(Timestep ts) = 0;
        virtual void OnEvent(Event& event) = 0;
        virtual void OnGuiRender() = 0;

    protected:
        std::string Name;
    };
}