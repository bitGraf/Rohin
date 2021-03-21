#include <enpch.hpp>

#include "OpenALContext.hpp"

namespace Engine {

    Ref<SoundContext> SoundContext::Create(Ref<SoundDevice> device) {
        return std::make_shared<OpenALContext>(device);
    }

    OpenALContext::OpenALContext(Ref<SoundDevice> device) : m_context(nullptr) {
        m_context = alcCreateContext((ALCdevice*)device->GetNativeDevice(), nullptr);
        if (!device->CheckError() || !m_context) {
            ENGINE_LOG_ERROR("Failed to create OpenAL context");
        }

        alListener3f(AL_POSITION, 0, 0, 0);
        alListener3f(AL_VELOCITY, 0, 0, 0);
    }

    OpenALContext::~OpenALContext() {
        m_context = nullptr;
    }

    void OpenALContext::MakeCurrent() const {
        auto res = alcMakeContextCurrent(m_context);
        if (!res) {
            ENGINE_LOG_ERROR("Failed to make OpenAL context current");
            return;
        }
    }

    void OpenALContext::Destroy() {
        MakeCurrent();
        alcDestroyContext(m_context);
    }

    void OpenALContext::SetListenerPosition(math::vec3 position) const {
        alListener3f(AL_POSITION, position.x, position.y, position.z);
    }

    void OpenALContext::SetListenerVelocity(math::vec3 velocity) const {
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    void OpenALContext::SetListenerOrientation(math::vec3 at, math::vec3 up) const {
        ALfloat vector[6] = { at.x, at.y, at.z, up.x, up.y, up.z };
        alListenerfv(AL_ORIENTATION, vector);
    }
}