#include <enpch.hpp>

#include "OpenALDevice.h"

namespace Engine {
    std::vector<std::string> SoundDevice::GetDevices() {
        std::vector<std::string> devicesVec;
        const ALCchar* devices;
        devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
        const char* ptr = devices;

        do {
            devicesVec.push_back(std::string(ptr));
            ptr += devicesVec.back().size() + 1;
        } while (*(ptr + 1) != '\0');

        return devicesVec;
    }

    Ref<SoundDevice> SoundDevice::Create() {
        return std::make_shared<OpenALSoundDevice>();
    }

    OpenALSoundDevice::OpenALSoundDevice() : m_device(nullptr) {

    }

    OpenALSoundDevice::~OpenALSoundDevice() {
        m_device = nullptr;
    }

    void OpenALSoundDevice::Open(const char* devicename) {
        m_device = alcOpenDevice(devicename);
        if (!CheckError() || !m_device) {
            // Failed to open device!!!
            ENGINE_LOG_ERROR("Failed to open OpenAL-soft Device!");
        }
    }

    void OpenALSoundDevice::Close() {
        if (!alcCloseDevice(m_device)) {
            // Failed to close device
        }
        m_device = nullptr;
    }

    bool OpenALSoundDevice::CheckError() const {
        ALCenum error = alcGetError(m_device);
        if (error != ALC_NO_ERROR)
        {
            ENGINE_LOG_ERROR("OpenAL-soft Error");
            //std::cerr << "***ERROR*** (" << __FILE__ << ": " << __LINE__ << ")\n";
            switch (error)
            {
            case ALC_INVALID_VALUE:
                std::cerr << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                break;
            case ALC_INVALID_DEVICE:
                std::cerr << "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
                break;
            case ALC_INVALID_CONTEXT:
                std::cerr << "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
                break;
            case ALC_INVALID_ENUM:
                std::cerr << "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
                break;
            case ALC_OUT_OF_MEMORY:
                std::cerr << "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
                break;
            default:
                std::cerr << "UNKNOWN ALC ERROR: " << error;
            }
            std::cerr << std::endl;
            return false;
        }
        return true;
    }
}