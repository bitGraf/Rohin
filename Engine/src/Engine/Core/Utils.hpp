#ifndef UTILS_H
#define UTILS_H

#include "Engine/Core/GameMath.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Benchmark.hpp"

#include "picojson.h"

typedef picojson::object& jsonObj;

template<typename T>
T safeAccess(jsonObj node, std::string key, T defaultVal) {
    if (node.find(key) != node.end()) {
        return node[key].get<T>();
    }
    return defaultVal;
}

template<typename T>
T safeAccessVec(jsonObj node, std::string key, T defaultVec) {
    if (node.find(key) != node.end()) {
        if (node[key].is<picojson::array>()) {
            return T(node[key].get<picojson::array>());
        }
    }
    return defaultVec;
}

/// Convert character string to Unsigned 32-bit Hash
u32 hash_djb2(unsigned char* str);

/// Convert character string to Unsigned 32-bit Hash
stringID operator"" _sid(const char *input, size_t s);


/// Strip string of all line comments (#) as well as block comments (/* */)
char* StripComments(char* inputBuf, size_t inputBufSize, size_t& newBufSize);
bool stringContainsChar(char* str, char ch);
size_t findChar(char* buffer, char ch);

/// Moving Average class. SIZE is number of samples to average over.
template<typename TYPE, int SIZE>
class MovingAverage {
    TYPE m_samples[SIZE];
    TYPE m_sum;
    u32 m_curSample;
    u32 m_sampleCount;

public:
    MovingAverage() :
        m_sum(static_cast<TYPE>(0)),
        m_curSample(0),
        m_sampleCount(0)
    {
    }

    void addSample(TYPE data) {
        if (m_sampleCount == SIZE) {
            m_sum -= m_samples[m_curSample];
        }
        else {
            m_sampleCount++;
        }

        m_samples[m_curSample] = data;
        m_sum += data;
        m_curSample++;

        if (m_curSample >= SIZE) {
            m_curSample = 0;
        }
    }
    f32 getCurrentAverage() const {
        if (m_sampleCount != 0) {
            return static_cast<f32>(m_sum) / static_cast<f32>(m_sampleCount);
        }

        return 0;
    }
};

#endif
