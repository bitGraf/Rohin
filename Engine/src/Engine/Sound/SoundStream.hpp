#pragma once

#include "Engine/Core/Base.hpp"

namespace Engine {

    class SoundStream {
    public:

        virtual void StartStream() const = 0;
        virtual void StopStream() const = 0;
        virtual void PauseStream() const = 0;
        virtual void ResumeStream() const = 0;

        virtual void UpdateStream(float dt) = 0;
        virtual void SetEffectSlot(u32 slot) = 0;

        virtual void SetPosition(float x, float y, float z) = 0;
        virtual void SetVelocty(float vx, float vy, float vz) = 0;

        static Ref<SoundStream> Create(const std::string& filename);
    };
}