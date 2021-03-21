#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Sound/SoundDevice.hpp"

namespace Engine {

    class SoundEffect {
    public:

        static void Init(Ref<SoundDevice> device);
        static void Destroy();

        static u32 GetReverbSlot();
    };
}