#include <enpch.hpp>

#include "SoundEngine.hpp"

#include "Engine/Sound/SoundDevice.hpp"
#include "Engine/Sound/SoundContext.hpp"
#include "Engine/Sound/SoundBuffer.hpp"
#include "Engine/Sound/SoundSource.hpp"

namespace Engine {

    struct SoundEngineData {
        //ALuint source;

        Ref<SoundDevice> device;
        Ref<SoundContext> context;
        Ref<SoundBuffer> buffer;
        Ref<SoundSource> source;
    };

    static SoundEngineData s_SoundData;
    
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
        int& size)
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
        int& size)
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
        auto devices = SoundDevice::GetDevices();
        ENGINE_LOG_INFO("----------------------------");
        ENGINE_LOG_INFO("Sound devices found: ");
        for (auto dev : devices) {
            ENGINE_LOG_TRACE(">  {0}", dev);
        }
        ENGINE_LOG_INFO("----------------------------");
        
        s_SoundData.device = SoundDevice::Create();
        s_SoundData.device->Open();
        
        s_SoundData.context = SoundContext::Create(s_SoundData.device);
        s_SoundData.context->MakeCurrent();
        
        // load wav file
        std::uint8_t channels;
        std::int32_t sampleRate;
        std::uint8_t bitsPerSample;
        int size;
        char* soundData = load_wav("run_tree/Data/Sounds/sound.wav", channels, sampleRate, bitsPerSample, size);
        if (soundData == nullptr)
        {
            std::cerr << "ERROR: Could not load wav" << std::endl;
            return;
        }
        
        
        int format;
        if (channels == 1 && bitsPerSample == 8)
            format = 0x1100;// AL_FORMAT_MONO8;
        else if (channels == 1 && bitsPerSample == 16)
            format = 0x1101; // AL_FORMAT_MONO16;
        else if (channels == 2 && bitsPerSample == 8)
            format = 0x1102; // AL_FORMAT_STEREO8;
        else if (channels == 2 && bitsPerSample == 16)
            format = 0x1103; //AL_FORMAT_STEREO16;
        else
        {
            std::cerr
                << "ERROR: unrecognised wave format: "
                << channels << " channels, "
                << bitsPerSample << " bps" << std::endl;
            return;
        }
        
        s_SoundData.buffer = SoundBuffer::Create();
        s_SoundData.buffer->BufferData(format, soundData, size, sampleRate);
    }

    void SoundEngine::StartSource() {
        s_SoundData.source = SoundSource::Create();

        s_SoundData.source->SetPitch(1.0f);
        s_SoundData.source->SetGain(0.25f);
        s_SoundData.source->SetPosition(0, 0, 0);
        s_SoundData.source->SetVelocity(0, 0, 0);
        s_SoundData.source->SetLooping(false);
        s_SoundData.source->SetBuffer(s_SoundData.buffer);

        s_SoundData.source->Play();
    }

    void SoundEngine::Update(double dt) {
        if (s_SoundData.source && !s_SoundData.source->IsPlaying()) {
            s_SoundData.source->Destroy();
        }
    }

    void SoundEngine::Shutdown() {
        s_SoundData.buffer->Destroy();
        
        s_SoundData.context->Destroy();
        
        s_SoundData.device->Close();
    }
}