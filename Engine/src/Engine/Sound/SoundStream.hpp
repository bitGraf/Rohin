#pragma once

#include "Engine/Core/Base.hpp"

namespace Engine {

    class SoundStream {
    public:

        virtual void PlayStream() const = 0;
        virtual void PauseStream() const = 0;
        virtual void StopStream() const = 0;
        virtual void UpdateStream(float dt) = 0;
        virtual void SetEffectSlot(u32 slot) = 0;

        static Ref<SoundStream> Create(const std::string& filename);
    };
}