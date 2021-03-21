#include <enpch.hpp>

#include "OpenALSoundStream.hpp"
#include "AL/efx.h"

namespace Engine {

    Ref<SoundStream> SoundStream::Create(const std::string& filename) {
        return std::make_shared<OpenALSoundStream>(filename);
    }

    OpenALSoundStream::OpenALSoundStream(const std::string& filename) {
        int error;
        vorbis_ptr = load_ogg_header(filename.c_str(), &error);
        m_filename = std::string(filename);

        //int bitsPerSample = 16;
        //int samplesPerBuffer = m_BufferSize * 8 / bitsPerSample;
        ALenum format;
        switch (get_ogg_channels(vorbis_ptr)) {
            case 1: format = AL_FORMAT_MONO16; break;
            case 2: format = AL_FORMAT_STEREO16; break;
        }

        alGenSources(1, &m_source);
        alSourcef(m_source, AL_PITCH, 1);
        alSourcef(m_source, AL_GAIN, 0.6f);
        alSource3f(m_source, AL_POSITION, 0, 0, 0);
        alSource3f(m_source, AL_VELOCITY, 0, 0, 0);
        alSourcei(m_source, AL_LOOPING, AL_FALSE);
        
        alGenBuffers(m_NumBuffers, &m_buffers[0]);
        short* data = (short*)malloc(m_BufferSize);
        //int num = get_ogg_samples(vorb, buffer, m_BufferSize);
        for (int n = 0; n < m_NumBuffers; n++) {
            auto bytes = sizeof(short) * get_ogg_samples(vorbis_ptr, data, m_BufferSize);

            alBufferData(m_buffers[n], format, data, bytes, get_ogg_sampleRate(vorbis_ptr));
        }

        alSourceQueueBuffers(m_source, m_NumBuffers, &m_buffers[0]);

        free(data);
    }

    OpenALSoundStream::~OpenALSoundStream() {}

    void OpenALSoundStream::PlayStream() const {
        alSourceStop(m_source);
        alSourcePlay(m_source);
    }

    void OpenALSoundStream::PauseStream() const {
        alSourcePause(m_source);
    }

    void OpenALSoundStream::StopStream() const {
        alSourceStop(m_source);
    }

    void OpenALSoundStream::SetEffectSlot(u32 slot) {
        alSource3i(m_source, AL_AUXILIARY_SEND_FILTER, (ALint)slot, 0, AL_FILTER_NULL);
        assert(alGetError() == AL_NO_ERROR && "Failed to setup sound source");
    }

    void OpenALSoundStream::UpdateStream(float dt) {
        m_time += dt;
        ALint buffersProcessed = 0;
        alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &buffersProcessed);
        if (buffersProcessed <= 0) {
            return;
        }

        while (buffersProcessed--) {
            printf("%d buffers done (%.2f)\n", buffersProcessed+1, m_time);

            ALuint buffer;
            alSourceUnqueueBuffers(m_source, 1, &buffer);

            short* data = (short*)malloc(m_BufferSize);
            memset(data, 0, m_BufferSize);

            auto bytes = sizeof(short) * get_ogg_samples(vorbis_ptr, data, m_BufferSize);

            ALenum format;
            switch (get_ogg_channels(vorbis_ptr)) {
                case 1: format = AL_FORMAT_MONO16; break;
                case 2: format = AL_FORMAT_STEREO16; break;
            }
            alBufferData(buffer, format, data, bytes, get_ogg_sampleRate(vorbis_ptr));
            alSourceQueueBuffers(m_source, 1, &buffer);

            if (bytes < m_BufferSize) {
                // reached the end of the file, need to reset the vorbis decoder?
                printf("Done with sound, restarting\n");
                int error;
                ogg_seek_start(vorbis_ptr);
                m_time = 0;
            }

            ALint state;
            alGetSourcei(m_source, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING) {
                printf("he\n");
                alSourceStop(m_source);
                alSourcePlay(m_source);
            }

            free(data);
        }
    }
}