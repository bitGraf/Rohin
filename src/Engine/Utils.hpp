#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <thread>

#include "GameMath.hpp"
#include "Console.hpp"
#include "Benchmark.hpp"

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


struct MultiData {
    s32 intVal;
    f32 floatVal;
    bool boolVal;
    std::string strVal;
    math::vec4 vecVal;

    MultiData() : intVal(0), floatVal(0), boolVal(false), valid(false) {}
    MultiData(s32 v) : intVal(v), floatVal(0), boolVal(false), valid(true) {}
    MultiData(f32 v) : intVal(0), floatVal(v), boolVal(false), valid(true) {}
    MultiData(f64 v) : intVal(0), floatVal(v), boolVal(false), valid(true) {}
    MultiData(bool v) : intVal(0), floatVal(0), boolVal(v), valid(true) {}
    MultiData(char* v) : intVal(0), floatVal(0), boolVal(false), strVal(v), valid(true) {}
    MultiData(char* v, size_t l) : intVal(0), floatVal(0), boolVal(false), strVal(v, l), valid(true) {}
    MultiData(std::string v) : intVal(0), floatVal(0), boolVal(false), strVal(v), valid(true) {}
    MultiData(math::vec4 v) : intVal(0), floatVal(0), boolVal(false), valid(true), vecVal(v) {}

    s32 asInt(int defaultVal = 0) { return valid ? intVal : defaultVal; }
    f32 asFloat(f32 defaultVal = 0.0f) { return valid ? floatVal : defaultVal; }
    bool asBool(bool defaultVal = false) { return valid ? boolVal : defaultVal; }
    std::string asString(std::string defaultVal = " ") { return valid ? strVal : defaultVal; }

    math::vec2 asVec2(math::vec2 defaultVal) { return valid ? math::vec2(vecVal.x, vecVal.y) : defaultVal; }
    math::vec3 asVec3(math::vec3 defaultVal) { return valid ? math::vec3(vecVal.x, vecVal.y, vecVal.z) : defaultVal; }
    math::vec4 asVec4(math::vec4 defaultVal) { return valid ? vecVal : defaultVal; }

    bool isValid() { return valid; }

private:
    bool valid;
};

struct DataNode {
    std::string name;

    MultiData getDataFromPath(std::string path);

    MultiData getData(std::string key);
    DataNode getChild(std::string key);
    //void OLD__CreateAsRoot(char* buffer, size_t bufSize);

    std::unordered_map<std::string, MultiData> data;
    std::unordered_map<std::string, DataNode> children;

    void decodeMultiDataStrings();
};

class Tokenizer {
public:
    Tokenizer(const char* _buffer) {
        fullSize = strlen(_buffer);
        __buffer = (char*)malloc(fullSize + 1);
        strcpy(__buffer, _buffer);
        __buffer[fullSize] = 0;

        ptr = __buffer;
    }
    Tokenizer(const std::string& str) {
        fullSize = str.size();
        __buffer = (char*)malloc(fullSize + 1);
        strcpy(__buffer, str.data());
        __buffer[fullSize] = 0;

        ptr = __buffer;
    }
    std::string NextToken(char* delim) {
        size_t loc = 0, len = strlen(ptr);
        for (size_t n = 0; n < len; n++) {
            for (size_t c = 0; c < strlen(delim); c++) {
                if (ptr[n] == delim[c]) {
                    std::string token(ptr, n);

                    ptr = ptr + n + 1;
                    return token;
                }
            }
        }
        return std::string(ptr);
    }

    std::string SplitByComma(bool* done) {
        size_t pos = 0, len = strlen(ptr);
        int count = -1;
        while (pos < len && count != 0) {
            if (ptr[pos] == '{') {
                if (count == -1)
                    count = 0;
                count++;
            }
            if (ptr[pos] == '}') {
                count--;
            }

            if (ptr[pos] == ',' && count == -1) {
                break;
            }

            pos++;
        }

        std::string group(ptr, pos);
        ptr = ptr + pos + 1;

        *done = pos == len;

        return group;
    }

    std::string SplitByColon() {
        // node1:{awk:ada,awk:ada}
        size_t pos = 0, len = strlen(ptr);
        int count = -1;
        while (pos < len && count != 0) {
            if (ptr[pos] == '{') {
                if (count == -1)
                    count = 0;
                count++;
            }
            if (ptr[pos] == '}') {
                count--;
            }

            if (ptr[pos] == ':' && count == -1) {
                break;
            }

            pos++;
        }

        std::string group(ptr, pos);
        ptr = ptr + pos + 1;

        return group;
    }

    ~Tokenizer() {
        ptr = __buffer;
        free(__buffer);
        __buffer = 0;
        fullSize = 0;
    }

private:
    char* __buffer;
    char* ptr;
    size_t fullSize;
};

void createNode(DataNode* me, DataNode* parent, std::string buffer);

#endif
