#ifndef ENGINE_H_pp
#define ENGINE_H_pp

#include "Engine/Core/Base.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Event/Event.hpp"
#include "Engine/Event/EventTypes.hpp"
#include "Engine/Core/Input.hpp"
#include "Engine/Gui/GuiLayer.hpp"
#include "Engine/Core/Timing.hpp"
#include "Engine/Scene/Scene.hpp"
#include "Engine/Core/MemoryTrack.hpp"

namespace Engine {

    class Application {
    public:
        Application();
        virtual ~Application();

        void Run();
        void Close();

        void HandleEvent(Event& event);

        void PushNewScene(Scene* newScene);

        inline static Application& Get() { return *s_Instance; }
        inline Window& GetWindow() { return *m_Window; }

        //GuiLayer* GetGuiLayer() { return m_GuiLayer; }

    private:
        bool OnWindowClose(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);

    private:
        std::unique_ptr<Window> m_Window;
        GuiLayer* m_GuiLayer;
        bool m_Done = false;
        bool m_Minimized = false;

        float m_LastFrameTime = 0;

        //LayerStack m_layerStack;
        Scene* m_CurrentScene = nullptr;
        Scene* m_NextScene = nullptr;

    private:
        static Application * s_Instance;
    };

    Application* CreateApplication();
}

#endif
