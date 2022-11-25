#pragma once

#include "Engine/Sound/SoundDevice.hpp"

#include "AL/al.h"
#include "AL/alc.h"

namespace rh {

    class OpenALSoundDevice : public SoundDevice {
    public:
        OpenALSoundDevice();
        virtual ~OpenALSoundDevice();

        virtual void Open(const char* devicename) override;
        virtual void Close() override;

        virtual bool CheckError() const override;
        virtual void* GetNativeDevice() const override { return m_device; };

        static std::vector<std::string> GetDevices();
        static Ref<SoundDevice> Create();

    private:
        ALCdevice* m_device;
    };
}