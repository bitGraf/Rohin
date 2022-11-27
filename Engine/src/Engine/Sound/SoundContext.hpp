#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Sound/SoundDevice.hpp"

namespace rh {

    class SoundContext {
    public:

        virtual void MakeCurrent() const = 0;
        virtual void Destroy() = 0;

        virtual void* GetNativeContext() const = 0;

        virtual void SetListenerPosition(laml::Vec3 position) const = 0;
        virtual void SetListenerVelocity(laml::Vec3 velocity) const = 0;
        virtual void SetListenerOrientation(laml::Vec3 at, laml::Vec3 up) const = 0;

        static Ref<SoundContext> Create(Ref<SoundDevice> device);
    };
}