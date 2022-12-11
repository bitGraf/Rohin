#include <enpch.hpp>

#include "SoundFileFormats.hpp"
#include "stb_vorbis.c"
#include "stb.h"

namespace rh {

    short* load_ogg(const std::string& filename,
        std::uint8_t& channels,
        std::int32_t& sampleRate,
        std::uint8_t& bitsPerSample,
        int& size) {

        int error;
        auto vorb = stb_vorbis_open_filename(filename.c_str(), &error, nullptr);
        auto vorb_info = stb_vorbis_get_info(vorb);

        int chan, sample_rate;
        short* output;
        int samples = stb_vorbis_decode_filename(filename.c_str(), &chan, &sample_rate, &output);
        if (samples < 0) {
            // failed to open/read .ogg file
            return nullptr;
        }

        channels = chan;
        sampleRate = sample_rate;
        bitsPerSample = sizeof(short)*8;
        size = samples * sizeof(short) * channels;
        return output;
    }

    void* load_ogg_header(const std::string& filename, int* error) {
        int error_;
        auto vorb = stb_vorbis_open_filename(filename.c_str(), &error_, nullptr);

        *error = error_;
        return (void*)vorb;
    }

    int get_ogg_samples(void* vorb, short* buffer, int buffer_size) {
        stb_vorbis* v = (stb_vorbis*)vorb;
        int num_shorts = buffer_size / 2;
        return (v->channels * stb_vorbis_get_samples_short_interleaved(v, v->channels, buffer, num_shorts));
    }

    int get_ogg_channels(void* vorb) {
        stb_vorbis* v = (stb_vorbis*)vorb;
        return v->channels;
    }

    int get_ogg_sampleRate(void* vorb) {
        stb_vorbis* v = (stb_vorbis*)vorb;
        return v->sample_rate;
    }

    void close_ogg_file(void* vorb) {
        stb_vorbis* v = (stb_vorbis*)vorb;
        stb_vorbis_close(v);
    }

    void ogg_seek_start(void* vorb) {
        stb_vorbis* v = (stb_vorbis*)vorb;
        stb_vorbis_seek_start(v);
    }

    short* load_ogg_test(const std::string& filename) {
        int error;
        auto vorb = stb_vorbis_open_filename(filename.c_str(), &error, nullptr);

        const int num_buffers = 4;
        const int buffer_size = 65536; //32 kB
        u32 buffers[num_buffers];
        int bitsPerSample = 16;
        int samplesPerBuffer = buffer_size * 8 / bitsPerSample;
        int num_shorts = buffer_size / 2;

        short* buffer = (short*)malloc(buffer_size);

        int num = stb_vorbis_get_samples_short_interleaved(vorb, vorb->channels, buffer, 32);

        free(buffer);

        return nullptr;
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
}