#pragma once

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