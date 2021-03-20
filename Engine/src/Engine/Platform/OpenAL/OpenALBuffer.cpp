#include <enpch.hpp>

#include "OpenALBuffer.hpp"

namespace Engine {

    Ref<SoundBuffer> SoundBuffer::Create(int format, const void* soundData, int numBytes, int sampleRate) {
        return std::make_shared<OpenALBuffer>(format, soundData, numBytes, sampleRate);
    }

    Ref<SoundBuffer> SoundBuffer::Create() {
        return std::make_shared<OpenALBuffer>();
    }

    OpenALBuffer::OpenALBuffer() : m_buffer(0) {
        alGenBuffers(1, &m_buffer);
    }

    OpenALBuffer::OpenALBuffer(ALenum format, const ALvoid* soundData, ALsizei numBytes, ALsizei sampleRate) : m_buffer(0) {
        alGenBuffers(1, &m_buffer);
        BufferData(format, soundData, numBytes, sampleRate);
    }

    void OpenALBuffer::BufferData(ALenum format, const ALvoid* soundData, ALsizei numBytes, ALsizei sampleRate) {
        alBufferData(m_buffer, format, soundData, numBytes, sampleRate);
    }

    OpenALBuffer::~OpenALBuffer() {
    }

    void OpenALBuffer::Destroy() {
        alDeleteBuffers(1, &m_buffer);
        m_buffer = 0;
    }
}