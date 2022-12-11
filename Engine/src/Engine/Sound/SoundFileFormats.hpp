#pragma once

#include "Engine/Core/Base.hpp"

namespace rh {

    short* load_ogg(const std::string& filename,
        std::uint8_t& channels,
        std::int32_t& sampleRate,
        std::uint8_t& bitsPerSample,
        int& size);

    void* load_ogg_header(const std::string& filename, int* error);
    int get_ogg_samples(void* vorb, short* buffer, int buffer_size);
    int get_ogg_channels(void* vorb);
    int get_ogg_sampleRate(void* vorb);
    void ogg_seek_start(void* vorb);
    void close_ogg_file(void* vorb);

    short* load_ogg_test(const std::string& filename);

    char* load_wav(const std::string& filename,
        std::uint8_t& channels,
        std::int32_t& sampleRate,
        std::uint8_t& bitsPerSample,
        int& size);
}