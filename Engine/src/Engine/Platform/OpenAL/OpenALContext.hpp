#pragma once

#include "Engine/Sound/SoundContext.hpp"

#include "AL/al.h"
#include "AL/alc.h"

namespace Engine {

    class OpenALContext : public SoundContext {
    public:

        OpenALContext(Ref<SoundDevice> device);
        virtual ~OpenALContext();

        virtual void MakeCurrent() const override;
        virtual void Destroy() override;

        virtual void* GetNativeContext() const { return m_context; }

    private:
        ALCcontext* m_context;
    };
}