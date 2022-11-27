#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Sound/SoundContext.hpp"

#include "AL/al.h"
#include "AL/alc.h"

namespace rh {

    class OpenALContext : public SoundContext {
    public:

        OpenALContext(Ref<SoundDevice> device);
        virtual ~OpenALContext();

        virtual void MakeCurrent() const override;
        virtual void Destroy() override;

        virtual void SetListenerPosition(laml::Vec3 position) const override;
        virtual void SetListenerVelocity(laml::Vec3 velocity) const override;
        virtual void SetListenerOrientation(laml::Vec3 at, laml::Vec3 up) const override;

        virtual void* GetNativeContext() const { return m_context; }

    private:
        ALCcontext* m_context;
    };
}