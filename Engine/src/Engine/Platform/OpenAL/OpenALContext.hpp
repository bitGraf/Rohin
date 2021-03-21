#pragma once

#include "Engine/Sound/SoundContext.hpp"
#include "Engine/Core/GameMath.hpp"

#include "AL/al.h"
#include "AL/alc.h"

namespace Engine {

    class OpenALContext : public SoundContext {
    public:

        OpenALContext(Ref<SoundDevice> device);
        virtual ~OpenALContext();

        virtual void MakeCurrent() const override;
        virtual void Destroy() override;

        virtual void SetListenerPosition(math::vec3 position) const override;
        virtual void SetListenerVelocity(math::vec3 velocity) const override;
        virtual void SetListenerOrientation(math::vec3 at, math::vec3 up) const override;

        virtual void* GetNativeContext() const { return m_context; }

    private:
        ALCcontext* m_context;
    };
}