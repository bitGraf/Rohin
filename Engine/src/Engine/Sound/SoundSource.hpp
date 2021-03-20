#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Sound/SoundBuffer.hpp"

namespace Engine {

    class SoundSource {
    public:

        virtual void Destroy() = 0;

        virtual void Play() = 0;
        virtual void Pause() = 0;
        virtual bool IsPlaying() = 0;

        virtual void SetPitch(float pitch) = 0;
        virtual float GetPitch() const = 0;
        virtual void SetGain(float gain) = 0;
        virtual float GetGain() const = 0;
        virtual void SetPosition(float x, float y, float z) = 0;
        virtual float* GetPosition() const = 0;
        virtual void SetVelocity(float vx, float vy, float vz) = 0;
        virtual float* GetVelocity() const = 0;
        virtual void SetLooping(bool looping) = 0;
        virtual bool GetLooping() const = 0;
        virtual void SetBuffer(Ref<SoundBuffer> buffer) = 0;
        virtual Ref<SoundBuffer> GetBuffer() const = 0;
        
        static Ref<SoundSource> Create();
    };
}