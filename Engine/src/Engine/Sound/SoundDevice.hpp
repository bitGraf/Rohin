#pragma once

#include "Engine/Core/Base.hpp"

namespace rh {

    class SoundDevice {
    public:

        virtual void Open(const char* devicename = nullptr) = 0;
        virtual void Close() = 0;

        virtual bool CheckError() const = 0;
        virtual void* GetNativeDevice() const = 0;

        static std::vector<std::string> GetDevices();
        static Ref<SoundDevice> Create();
    };
}