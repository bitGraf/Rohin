#pragma once
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/GraphicsContext.hpp"

#include <GLFW/glfw3.h>

namespace rh {

    class Window_windows : public Window
    {
    public:
        Window_windows(std::string title, u32 height, u32 width);
        virtual ~Window_windows();

        void Update() override;

        u32 GetWidth() const override { return m_data.Width; }
        u32 GetHeight() const override { return m_data.Height; }
        u32 GetXpos() const override { return m_data.Xpos; }
        u32 GetYpos() const override { return m_data.Ypos; }

        void SetEventCallback(const EventCallbackFcn& callback) override { m_data.EventCallback = callback; }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        inline virtual void* GetNativeWindow() const override { return (void*)m_glfwWindow; }
        inline virtual GraphicsContext* GetGraphicsContext() const override { return m_Context; }

    private:
        /* Startup and shutdown functions */
        virtual void Init(std::string title, u32 height, u32 width);
        virtual void Shutdown();

        /* glfw handle */
        GLFWwindow* m_glfwWindow;
        GraphicsContext* m_Context;

        /* for use with glfw getUserDataPointer */
        struct WindowData {
            std::string Title;
            u32 Width, Height;
            u32 Xpos, Ypos;
            bool VSync;

            EventCallbackFcn EventCallback;
        };

        WindowData m_data;
    };
}

