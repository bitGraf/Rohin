#pragma once

#include "Engine/Core/Base.hpp"

namespace rh {

    enum SoundFormat {
        None = -1,

        Mono8,
        Mono16,

        Stereo8,
        Stereo16
    };

    SoundFormat GetSoundFormat(u8 channels, u8 bitsPerSample);

    class SoundBuffer {
    public:

        virtual void Destroy() = 0;

        virtual void BufferData(SoundFormat format, const void* soundData, int numBytes, int sampleRate) = 0;

        virtual u32 GetNativeID() const = 0;
        virtual float GetLength_s() const = 0;

        // Create empty buffer
        static Ref<SoundBuffer> Create();

        // Create and fill buffer
        static Ref<SoundBuffer> Create(SoundFormat format, const void* soundData, int numBytes, int sampleRate);
    };
}