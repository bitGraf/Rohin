#include "Utils.hpp"

std::string getNextString(std::istringstream& iss) {
    std::string str;
    std::getline(iss, str, '"');
    std::getline(iss, str, '"');

    return str;
}

math::scalar getNextFloat(std::istringstream& iss) {
    math::scalar v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v;
    std::getline(iss, garb, '"');

    return v;
}

math::vec2 getNextVec2(std::istringstream& iss) {
    math::vec2 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y;
    std::getline(iss, garb, '"');

    return v;
}

math::vec3 getNextVec3(std::istringstream& iss) {
    math::vec3 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y >> v.z;
    std::getline(iss, garb, '"');

    return v;
}

math::vec4 getNextVec4(std::istringstream& iss) {
    math::vec4 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y >> v.z >> v.w;
    std::getline(iss, garb, '"');

    return v;
}

/*
template MovingAverage<long long, 10>::MovingAverage();
template void MovingAverage<long long, 10>::addSample(long long data);
template f32 MovingAverage<long long, 10>::getCurrentAverage() const;

template<typename TYPE, int SIZE>
MovingAverage<TYPE, SIZE>::MovingAverage() :
    m_sum(static_cast<TYPE>(0)),
    m_curSample(0),
    m_sampleCount(0)
{
}

template<typename TYPE, int SIZE>
void MovingAverage<TYPE, SIZE>::addSample(TYPE data) {
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

template<typename TYPE, int SIZE>
f32 MovingAverage<TYPE, SIZE>::getCurrentAverage() const {
    if (m_sampleCounr != 0) {
        return static_cast<f32>(m_sum) / static_cast<f32>(m_sampleCount);
    }

    return 0;
}
*/