#pragma once

#include "Engine/Core/Base.hpp"

namespace Engine {

    class SoundBuffer {
    public:

        virtual void Destroy() = 0;

        virtual void BufferData(int format, const void* soundData, int numBytes, int sampleRate) = 0;

        virtual u32 GetNativeID() const = 0;

        // Create empty buffer
        static Ref<SoundBuffer> Create();

        // Create and fill buffer
        static Ref<SoundBuffer> Create(int format, const void* soundData, int numBytes, int sampleRate);
    };
}