#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Core/GameMath.hpp"

#include "Engine/Sound/SoundDevice.hpp"

namespace rh {

    class SoundContext {
    public:

        virtual void MakeCurrent() const = 0;
        virtual void Destroy() = 0;

        virtual void* GetNativeContext() const = 0;

        virtual void SetListenerPosition(math::vec3 position) const = 0;
        virtual void SetListenerVelocity(math::vec3 velocity) const = 0;
        virtual void SetListenerOrientation(math::vec3 at, math::vec3 up) const = 0;

        static Ref<SoundContext> Create(Ref<SoundDevice> device);
    };
}