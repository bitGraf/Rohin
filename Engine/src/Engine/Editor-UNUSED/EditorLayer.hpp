    /*
#pragma once

#include "Engine/EngineLayer.hpp"
#include "Engine/Event/EventTypes.hpp"
#include "Engine/Editor/SceneHierarchyPanel.hpp"
#include "Engine/Renderer/Framebuffer.hpp"

namespace Engine {

    class EditorLayer : public EngineLayer {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        virtual void OnUpdate(Timestep ts) override;
        virtual void OnEvent(Event& event) override;
        virtual void OnGuiRender() override;

        //virtual Ref<Scene> GetScene() override { return m_EditorScene; }

        void SetGameLayer(EngineLayer* layer) { m_GameLayer = layer; }

    private:
        void CheckViewportSize();
        bool OnKeyPressedEvent(KeyPressedEvent& e);

    private:

        SceneHierarchyPanel m_Panel;
        Ref<Framebuffer> m_Framebuffer;

        //Ref<Scene> m_EditorScene;
        GameObject m_EditorCamera;

        // Viewport data
        math::vec2 m_ViewportSize;
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;

        //bool m_EditorEnabled = true;

        EngineLayer* m_GameLayer;
    };
}
    */