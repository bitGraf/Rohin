#pragma once

#include "Engine/Sound/SoundBuffer.hpp"

#include "AL/al.h"
#include "AL/alc.h"

namespace rh {

    class OpenALBuffer : public SoundBuffer {
    public:

        OpenALBuffer();
        OpenALBuffer(SoundFormat format, const ALvoid* soundData, ALsizei numBytes, ALsizei sampleRate);
        virtual ~OpenALBuffer();

        virtual void BufferData(SoundFormat format, const void* soundData, int numBytes, int sampleRate) override;
        virtual void Destroy() override;
        virtual float GetLength_s() const override { return (float)m_numSamples / (float)m_sampleRate; }

        virtual u32 GetNativeID() const { return m_buffer; }

    private:
        ALuint m_buffer;
        u8 m_numChannels, m_bitsPerSample;
        ALsizei m_sampleRate;
        ALsizei m_numSamples;
    };
}