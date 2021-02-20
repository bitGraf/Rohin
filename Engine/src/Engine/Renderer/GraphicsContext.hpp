#pragma once

namespace Engine {

    class GraphicsContext {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        virtual const unsigned char* GetVendorString() const = 0;
        virtual const unsigned char* GetDeviceString() const = 0;
        virtual const unsigned char* GetVersionString() const = 0;
        virtual const unsigned char* GetAPIVersionString() const = 0;
    };
}