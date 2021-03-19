#include <enpch.hpp>

#include "Sound.hpp"
#include "AL/al.h"
#include "AL/alc.h"

namespace Engine {

    struct SoundEngineData {
        ALCdevice* openALDevice = nullptr; // audio device
        ALCcontext* openALContext = nullptr; // openAL context?
        ALuint buffer;
        ALuint source;
    };

    static SoundEngineData s_SoundData;

#define checkError(device) check_alc_errors(__FILE__, __LINE__, device)

    bool check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device)
    {
        ALCenum error = alcGetError(device);
        if (error != ALC_NO_ERROR)
        {
            std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n";
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

    std::int32_t convert_to_int(char* buffer, std::size_t len)
    {
        std::int32_t a = 0;
        if (true/*std::endian::native == std::endian::little*/)
            std::memcpy(&a, buffer, len);
        else
            for (std::size_t i = 0; i < len; ++i)
                reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
        return a;
    }

    bool load_wav_file_header(std::ifstream& file,
        std::uint8_t& channels,
        std::int32_t& sampleRate,
        std::uint8_t& bitsPerSample,
        ALsizei& size)
    {
        char buffer[4];
        if (!file.is_open())
            return false;

        // the RIFF
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read RIFF" << std::endl;
            return false;
        }
        if (std::strncmp(buffer, "RIFF", 4) != 0)
        {
            std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
            return false;
        }

        // the size of the file
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read size of file" << std::endl;
            return false;
        }

        // the WAVE
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read WAVE" << std::endl;
            return false;
        }
        if (std::strncmp(buffer, "WAVE", 4) != 0)
        {
            std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
            return false;
        }

        // "fmt/0"
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read fmt/0" << std::endl;
            return false;
        }

        // this is always 16, the size of the fmt data chunk
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read the 16" << std::endl;
            return false;
        }

        // PCM should be 1?
        if (!file.read(buffer, 2))
        {
            std::cerr << "ERROR: could not read PCM" << std::endl;
            return false;
        }

        // the number of channels
        if (!file.read(buffer, 2))
        {
            std::cerr << "ERROR: could not read number of channels" << std::endl;
            return false;
        }
        channels = convert_to_int(buffer, 2);

        // sample rate
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read sample rate" << std::endl;
            return false;
        }
        sampleRate = convert_to_int(buffer, 4);

        // (sampleRate * bitsPerSample * channels) / 8
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
            return false;
        }

        // ?? dafaq
        if (!file.read(buffer, 2))
        {
            std::cerr << "ERROR: could not read dafaq" << std::endl;
            return false;
        }

        // bitsPerSample
        if (!file.read(buffer, 2))
        {
            std::cerr << "ERROR: could not read bits per sample" << std::endl;
            return false;
        }
        bitsPerSample = convert_to_int(buffer, 2);

        // data chunk header "data"
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read data chunk header" << std::endl;
            return false;
        }
        if (std::strncmp(buffer, "data", 4) != 0)
        {
            std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
            return false;
        }

        // size of data
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read data size" << std::endl;
            return false;
        }
        size = convert_to_int(buffer, 4);

        /* cannot be at the end of file */
        if (file.eof())
        {
            std::cerr << "ERROR: reached EOF on the file" << std::endl;
            return false;
        }
        if (file.fail())
        {
            std::cerr << "ERROR: fail state set on the file" << std::endl;
            return false;
        }

        return true;
    }

    char* load_wav(const std::string& filename,
        std::uint8_t& channels,
        std::int32_t& sampleRate,
        std::uint8_t& bitsPerSample,
        ALsizei& size)
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open())
        {
            std::cerr << "ERROR: Could not open \"" << filename << "\"" << std::endl;
            return nullptr;
        }
        if (!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
        {
            std::cerr << "ERROR: Could not load wav header of \"" << filename << "\"" << std::endl;
            return nullptr;
        }

        char* data = new char[size];

        in.read(data, size);

        return data;
    }

    void SoundEngine::Init() {
        s_SoundData.openALDevice = alcOpenDevice(nullptr);
        if (!s_SoundData.openALDevice)
        {
            __debugbreak();
        }

        std::vector<std::string> devicesVec;
        const ALCchar* devices;
        devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
        const char* ptr = devices;

        do {
            devicesVec.push_back(std::string(ptr));
            ptr += devicesVec.back().size() + 1;
        } while (*(ptr + 1) != '\0');

        ENGINE_LOG_INFO("OpenAL Devices:");
        for (const auto& str : devicesVec) {
            ENGINE_LOG_INFO("  {0}", str);
        }
        ENGINE_LOG_INFO("----------------------------");

        s_SoundData.openALContext = alcCreateContext(s_SoundData.openALDevice, nullptr);

        ALCboolean contextMadeCurrent = false;
        contextMadeCurrent = alcMakeContextCurrent(s_SoundData.openALContext);
        if (contextMadeCurrent != ALC_TRUE) {
            __debugbreak();
        }

        // load wav file
        std::uint8_t channels;
        std::int32_t sampleRate;
        std::uint8_t bitsPerSample;
        ALsizei size;
        char* soundData = load_wav("run_tree/Data/Sounds/sound.wav", channels, sampleRate, bitsPerSample, size);
        if (soundData == nullptr)
        {
            std::cerr << "ERROR: Could not load wav" << std::endl;
            return;
        }

        alGenBuffers(1, &s_SoundData.buffer);
        checkError(s_SoundData.openALDevice);

        ALenum format;
        if (channels == 1 && bitsPerSample == 8)
            format = AL_FORMAT_MONO8;
        else if (channels == 1 && bitsPerSample == 16)
            format = AL_FORMAT_MONO16;
        else if (channels == 2 && bitsPerSample == 8)
            format = AL_FORMAT_STEREO8;
        else if (channels == 2 && bitsPerSample == 16)
            format = AL_FORMAT_STEREO16;
        else
        {
            std::cerr
                << "ERROR: unrecognised wave format: "
                << channels << " channels, "
                << bitsPerSample << " bps" << std::endl;
            return;
        }

        alBufferData(s_SoundData.buffer, format, soundData, size, sampleRate);
        checkError(s_SoundData.openALDevice);
    }

    void SoundEngine::StartSource() {
        alGenSources(1, &s_SoundData.source);
        alSourcef(s_SoundData.source, AL_PITCH, 1);
        alSourcef(s_SoundData.source, AL_GAIN, 0.25f);
        alSource3f(s_SoundData.source, AL_POSITION, 0, 0, 0);
        alSource3f(s_SoundData.source, AL_VELOCITY, 20, 0, 0);
        alSourcei(s_SoundData.source, AL_LOOPING, AL_FALSE);
        alSourcei(s_SoundData.source, AL_BUFFER, s_SoundData.buffer);

        alSourcePlay(s_SoundData.source);
        checkError(s_SoundData.openALDevice);
    }

    void SoundEngine::Update(double dt) {
        ALint state = AL_PLAYING;
        alGetSourcei(s_SoundData.source, AL_SOURCE_STATE, &state);
        checkError(s_SoundData.openALDevice);

        if (state != AL_PLAYING) {
            // done with the sound.
            alDeleteSources(1, &s_SoundData.source);
        }
    }

    void SoundEngine::Shutdown() {
        alDeleteBuffers(1, &s_SoundData.buffer);

        alcMakeContextCurrent(s_SoundData.openALContext);
        alcDestroyContext(s_SoundData.openALContext);

        if (!alcCloseDevice(s_SoundData.openALDevice)) {
            ENGINE_LOG_ASSERT(false, "Failed to close OpenAL????");
        }
    }
}