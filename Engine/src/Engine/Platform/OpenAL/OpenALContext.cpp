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
    }

    OpenALContext::~OpenALContext() {
        m_context = nullptr;
    }

    void OpenALContext::MakeCurrent() const {
        auto res = alcMakeContextCurrent(m_context);
        if (!res) {
            ENGINE_LOG_ERROR("Failed to make OpenAL context current");
        }
    }

    void OpenALContext::Destroy() {
        MakeCurrent();
        alcDestroyContext(m_context);
    }
}