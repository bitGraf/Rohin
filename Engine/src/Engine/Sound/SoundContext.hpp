#pragma once

#include "Engine/Core/Base.hpp"

#include "Engine/Sound/SoundDevice.hpp"

namespace Engine {

    class SoundContext {
    public:

        virtual void MakeCurrent() const = 0;
        virtual void Destroy() = 0;

        virtual void* GetNativeContext() const = 0;

        static Ref<SoundContext> Create(Ref<SoundDevice> device);
    };
}