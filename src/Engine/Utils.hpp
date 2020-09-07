#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include "GameMath.hpp"
#include "Console.hpp"

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

bool stringContainsChar(char* str, char ch);


struct MultiData {
    s32 intVal;
    f32 floatVal;
    bool boolVal;
    std::string strVal;

    MultiData() : intVal(0), floatVal(0), boolVal(false), valid(false) {}
    MultiData(s32 v) : intVal(v), floatVal(0), boolVal(false), valid(true) {}
    MultiData(f32 v) : intVal(0), floatVal(v), boolVal(false), valid(true) {}
    MultiData(f64 v) : intVal(0), floatVal(v), boolVal(false), valid(true) {}
    MultiData(bool v) : intVal(0), floatVal(0), boolVal(v), valid(true) {}
    MultiData(char* v) : intVal(0), floatVal(0), boolVal(false), strVal(v), valid(true) {}
    MultiData(char* v, size_t l) : intVal(0), floatVal(0), boolVal(false), strVal(v, l), valid(true) {}
    MultiData(std::string v) : intVal(0), floatVal(0), boolVal(false), strVal(v), valid(true) {}

    s32 asInt() { return intVal; }
    f32 asFloat() { return floatVal; }
    bool asBool() { return boolVal; }
    std::string asString() { return strVal; }
    bool isValid() { return valid; }

private:
    bool valid;
};

struct DataNode {
    std::string name;

    MultiData getDataFromPath(std::string path);

    /* super temprary helper functions */
    math::vec2 getVec2(std::string path1, std::string path2);
    math::vec3 getVec3(std::string path1, std::string path2, std::string path3);

    MultiData getData(std::string key);
    DataNode getChild(std::string key);
    void CreateAsRoot(char* buffer, size_t bufSize);

    std::unordered_map<std::string, MultiData> data;
    std::unordered_map<std::string, DataNode> children;

private:
    void decodeMultiDataStrings();
};

#endif
