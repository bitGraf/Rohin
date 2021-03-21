#pragma once

#include "Engine/Sound/SoundStream.hpp"

#include "Engine/Sound/SoundFileFormats.hpp"
#include "AL/al.h"

namespace Engine {

    const size_t m_NumBuffers = 8;
    const ALsizei m_BufferSize = 65536; // 32kB

    class OpenALSoundStream : public SoundStream {
    public:

        OpenALSoundStream(const std::string& filename);
        virtual ~OpenALSoundStream();

        virtual void PlayStream() const override;
        virtual void PauseStream() const override;
        virtual void StopStream() const override;
        virtual void UpdateStream(float dt) override;

    private:

        u32 m_source;
        u32 m_buffers[m_NumBuffers];
        void* vorbis_ptr; // i guess this works
        std::string m_filename;
        bool done = false;
        float m_time = 0;
    };
}