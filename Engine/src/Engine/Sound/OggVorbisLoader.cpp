#include <enpch.hpp>

#include "stb_vorbis.c"
#include "stb.h"

namespace Engine {

    short* load_ogg(const std::string& filename,
        std::uint8_t& channels,
        std::int32_t& sampleRate,
        std::uint8_t& bitsPerSample,
        int& size) {

        int chan, sample_rate;
        short* output;
        int samples = stb_vorbis_decode_filename(filename.c_str(), &chan, &sample_rate, &output);

        channels = chan;
        sampleRate = sample_rate;
        bitsPerSample = sizeof(short)*8;
        size = samples * sizeof(short);
        return output;
    }
}