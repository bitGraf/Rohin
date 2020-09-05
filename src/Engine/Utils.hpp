#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

#include "GameMath.hpp"

std::string getNextString(std::istringstream& iss); /// Read next tokenized string from input string stream
math::scalar getNextFloat(std::istringstream& iss); /// Read next tokenized float from input string stream
math::vec2 getNextVec2(std::istringstream& iss);    /// Read next tokenized Vec2 from input string stream
math::vec3 getNextVec3(std::istringstream& iss);    /// Read next tokenized Vec3 from input string stream
math::vec4 getNextVec4(std::istringstream& iss);    /// Read next tokenized Vec4 from input string stream

/// Convert character string to Unsigned 32-bit Hash
u32 hash_djb2(unsigned char* str);

/// Convert character string to Unsigned 32-bit Hash
stringID operator"" _sid(const char *input, size_t s);


/// Strip string of all line comments (#) as well as block comments (/* */)
char* StripComments(char* inputBuf, size_t inputBufSize, size_t& newBufSize);


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


/* Key-Value heirarchy */
class KVH {
public:
    KVH();
    ~KVH();

    void Destroy();
    void CreateAsRoot(char* buffer, size_t bufferSize);

    void AddChild(u32 num);
    
    u32 numChildren;
    KVH** children;
private:
};

#endif
