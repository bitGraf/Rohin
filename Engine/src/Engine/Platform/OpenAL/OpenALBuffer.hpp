#pragma once

#include "Engine/Sound/SoundBuffer.hpp"

#include "AL/al.h"
#include "AL/alc.h"

namespace Engine {

    class OpenALBuffer : public SoundBuffer {
    public:

        OpenALBuffer();
        OpenALBuffer(SoundFormat format, const ALvoid* soundData, ALsizei numBytes, ALsizei sampleRate);
        virtual ~OpenALBuffer();

        virtual void BufferData(SoundFormat format, const void* soundData, int numBytes, int sampleRate) override;
        virtual void Destroy() override;

        virtual u32 GetNativeID() const { return m_buffer; }

    private:
        ALuint m_buffer;
    };
}