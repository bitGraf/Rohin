#include <enpch.hpp>

#include "OpenALBuffer.hpp"
#include "OpenALError.hpp"

namespace rh {

    Ref<SoundBuffer> SoundBuffer::Create(SoundFormat format, const void* soundData, int numBytes, int sampleRate) {
        return std::make_shared<OpenALBuffer>(format, soundData, numBytes, sampleRate);
    }

    Ref<SoundBuffer> SoundBuffer::Create() {
        return std::make_shared<OpenALBuffer>();
    }

    SoundFormat GetSoundFormat(u8 channels, u8 bitsPerSample) {
        if (channels == 1 && bitsPerSample == 8)
            return SoundFormat::Mono8;
        else if (channels == 1 && bitsPerSample == 16)
            return SoundFormat::Mono16;
        else if (channels == 2 && bitsPerSample == 8)
            return SoundFormat::Stereo8;
        else if (channels == 2 && bitsPerSample == 16)
            return SoundFormat::Stereo16;
        else
        {
            std::cerr
                << "ERROR: unrecognised sound format: "
                << channels << " channels, "
                << bitsPerSample << " bps" << std::endl;
            return SoundFormat::None;
        }
    }

    OpenALBuffer::OpenALBuffer() {
        alGenBuffers(1, &m_buffer);
        CheckAlError(__FILE__, __LINE__);
    }

    OpenALBuffer::OpenALBuffer(SoundFormat format, const ALvoid* soundData, ALsizei numBytes, ALsizei sampleRate) {
        alGenBuffers(1, &m_buffer);
        CheckAlError(__FILE__, __LINE__);

        BufferData(format, soundData, numBytes, sampleRate);
    }

    ALenum GetALFormat(SoundFormat format) {
        switch (format) {
        case SoundFormat::Mono8:
            return AL_FORMAT_STEREO8;
        case SoundFormat::Mono16:
            return AL_FORMAT_MONO16;
        case SoundFormat::Stereo8:
            return AL_FORMAT_STEREO8;
        case SoundFormat::Stereo16:
            return AL_FORMAT_STEREO16;
        case SoundFormat::None:
            return 0;
        }
        return 0;
    }

    void OpenALBuffer::BufferData(SoundFormat format, const ALvoid* soundData, ALsizei numBytes, ALsizei sampleRate) {
        ALenum alFormat = GetALFormat(format);
        alBufferData(m_buffer, alFormat, soundData, numBytes, sampleRate);
        CheckAlError(__FILE__, __LINE__);

        switch (format) {
        case SoundFormat::Mono8:
        case SoundFormat::Mono16:   m_numChannels = 1;
        case SoundFormat::Stereo8:
        case SoundFormat::Stereo16: m_numChannels = 2;
        }
        switch (format) {
        case SoundFormat::Mono8:
        case SoundFormat::Stereo8:  m_bitsPerSample = 8;
        case SoundFormat::Mono16:
        case SoundFormat::Stereo16: m_bitsPerSample = 16;
        }
        m_sampleRate = sampleRate;
        m_numSamples = numBytes * 8 / (m_bitsPerSample * m_numChannels);
    }

    OpenALBuffer::~OpenALBuffer() {
    }

    void OpenALBuffer::Destroy() {
        alDeleteBuffers(1, &m_buffer);
        CheckAlError(__FILE__, __LINE__);

        m_buffer = 0;
    }
}