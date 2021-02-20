#pragma once

#include "Engine/EngineLayer.hpp"

namespace Engine {

    class GuiLayer : public EngineLayer
    {
    public:
        GuiLayer();
        ~GuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& event);
        virtual void OnGuiRender() override;

        void Begin();
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }

    private:
        bool m_BlockEvents = true;
        float m_Time;
    };

}