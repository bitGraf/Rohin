#pragma once
#include <enpch.hpp>

#include "Engine/Core/Base.hpp"
#include "Engine/Event/Event.hpp"
#include "Engine/Renderer/GraphicsContext.hpp"

namespace rh {
    /* virtual class */
    class Window {
    public:
        using EventCallbackFcn = std::function<void(Event&)>;

        virtual ~Window() = default;

        /* Initialization functions*/
        virtual void Update() = 0;

        /* Access Window parameters */
        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;
        virtual u32 GetXpos() const = 0;
        virtual u32 GetYpos() const = 0;

        virtual void SetEventCallback(const EventCallbackFcn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0;
        virtual GraphicsContext* GetGraphicsContext() const = 0;

        /* Create a new window */
        static Window* Create(std::string title = "Window", u32 height = 720, u32 width = 1280);
    };
}