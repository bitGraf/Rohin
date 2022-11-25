#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Event/Event.hpp"

namespace rh {

    class GuiLayer
    {
    public:
        GuiLayer();
        ~GuiLayer();

        virtual void OnAttach();
        virtual void OnDetach();
        virtual void OnEvent(Event& event);
        virtual void OnGuiRender();

        void Begin();
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }

    private:
        bool m_BlockEvents = true;
        float m_Time;
    };

}