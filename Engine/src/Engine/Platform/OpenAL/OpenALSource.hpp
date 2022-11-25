#pragma once

#include "Engine/Sound/SoundSource.hpp"

#include "AL/al.h"
#include "AL/alc.h"

namespace rh {

    class OpenALSource : public SoundSource {
    public:

        OpenALSource();
        virtual ~OpenALSource();
        virtual void Destroy() override;

        virtual void Play() override;
        virtual void Pause() override;
        virtual void Stop() override;
        virtual bool IsPlaying() override;

        virtual void SetPitch(float pitch) override;
        virtual void SetGain(float gain) override;
        virtual void SetPosition(float x, float y, float z) override;
        virtual void SetVelocity(float vx, float vy, float vz) override;
        virtual void SetLooping(bool looping) override;
        virtual void SetBuffer(Ref<SoundBuffer> buffer) override;
        virtual void SetBuffer(u32 buffer) override;
        
        virtual float GetPitch() const override { return m_pitch; }
        virtual float GetGain() const override { return m_gain; }
        virtual float* GetPosition() const override { return (float*)m_position; }
        virtual float* GetVelocity() const override { return (float*)m_velocity; }
        virtual bool GetLooping() const override { return m_looping; }
        //virtual Ref<SoundBuffer> GetBuffer() const override { return m_buffer; }

        virtual u32 GetNativeID() const { return m_source; }

    private:
        ALuint m_source;

        ALboolean m_playing;

        ALfloat m_pitch, m_gain;
        ALfloat m_position[3];
        ALfloat m_velocity[3];
        ALboolean m_looping;
    };
}