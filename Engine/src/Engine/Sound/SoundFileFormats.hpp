#pragma once

#include "Engine/Core/Base.hpp"

namespace Engine {

    short* load_ogg(const std::string& filename,
        std::uint8_t& channels,
        std::int32_t& sampleRate,
        std::uint8_t& bitsPerSample,
        int& size);

    char* load_wav(const std::string& filename,
        std::uint8_t& channels,
        std::int32_t& sampleRate,
        std::uint8_t& bitsPerSample,
        int& size);
}