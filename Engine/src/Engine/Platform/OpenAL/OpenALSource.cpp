#include <enpch.hpp>

#include "OpenALSource.hpp"
#include "OpenALError.hpp"

#include "AL/al.h"

namespace Engine {

    Ref<SoundSource> SoundSource::Create() {
        return std::make_shared<OpenALSource>();
    }

    OpenALSource::OpenALSource() {
        alGenSources(1, &m_source);
        CheckAlError(__FILE__, __LINE__);

        // set reasonable defaults
        alSourcef(m_source, AL_PITCH, 1.0f);
        alSourcef(m_source, AL_GAIN, 1.0f);
        alSource3f(m_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(m_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        alSourcei(m_source, AL_LOOPING, AL_FALSE);
        CheckAlError(__FILE__, __LINE__);
    }

    OpenALSource::~OpenALSource() {
    }

    void OpenALSource::Destroy() {
        if (m_source == 0) return;

        alDeleteSources(1, &m_source);
        CheckAlError(__FILE__, __LINE__);

        m_source = 0;
    }

    void OpenALSource::Play() {
        if (m_source == 0) return;

        alSourcePlay(m_source);
        CheckAlError(__FILE__, __LINE__);
    }

    void OpenALSource::Pause() {

    }

    bool OpenALSource::IsPlaying() {
        if (m_source == 0) return false; // if source is destroyed?

        ALint state;
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);
        CheckAlError(__FILE__, __LINE__);
        m_playing = state == AL_PLAYING;
        return m_playing;
    }


    void OpenALSource::SetPitch(float pitch) {
        alSourcef(m_source, AL_PITCH, pitch);
    }

    void OpenALSource::SetGain(float gain) {
        alSourcef(m_source, AL_GAIN, gain);
    }

    void OpenALSource::SetPosition(float x, float y, float z) {
        alSource3f(m_source, AL_POSITION, x, y, z);
    }

    void OpenALSource::SetVelocity(float vx, float vy, float vz) {
        alSource3f(m_source, AL_VELOCITY, vx, vy, vz);
    }

    void OpenALSource::SetLooping(bool looping) {
        alSourcei(m_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
    }

    void OpenALSource::SetBuffer(Ref<SoundBuffer> buffer) {
        alSourcei(m_source, AL_BUFFER, buffer->GetNativeID());
    }

    void OpenALSource::SetBuffer(u32 buffer) {
        alSourcei(m_source, AL_BUFFER, buffer);
    }

}