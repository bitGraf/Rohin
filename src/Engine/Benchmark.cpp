#include "Benchmark.hpp"


ScopeTimer::ScopeTimer(const char* _name) : name(_name), done(false) {
    startTime = std::chrono::high_resolution_clock::now();
}

ScopeTimer::~ScopeTimer() {
    if (!done)
        Stop();
}

void ScopeTimer::Stop() {
    auto endTime = std::chrono::high_resolution_clock::now();

    long long start = std::chrono::time_point_cast<std::chrono::milliseconds>(startTime).time_since_epoch().count();
    long long end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTime).time_since_epoch().count();

    std::cout << name << ": " << (end - start) << " ms" << std::endl;

    done = true;
}




BenchmarkTimer::BenchmarkTimer(const char* _name) : name(_name), done(false) {
    startTime = std::chrono::high_resolution_clock::now();
}

BenchmarkTimer::~BenchmarkTimer() {
    if (!done)
        Stop();
}

void BenchmarkTimer::Stop() {
    auto endTime = std::chrono::high_resolution_clock::now();

    long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch().count();
    long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

    uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
    Benchmark::Get()->WriteProfile({name, start, end, threadID});

    done = true;
}



Benchmark::Benchmark() : currentSession(0), numProfiles(0) {}

void Benchmark::BeginSession(const std::string& name, const std::string& filepath) {
    outputStream.open(filepath);
    WriteHeader();
    currentSession = new BenchmarkSession{ name };
}

void Benchmark::EndSession() {
    WriteFooter();
    outputStream.close();
    delete currentSession;
    currentSession = 0;
    numProfiles = 0;
}

void Benchmark::WriteProfile(const ProfileResult& result) {
    if (outputStream.is_open()) {
        if (numProfiles++ > 0)
            outputStream << ",";

        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');
        auto k = name.find("__thiscall");
        if (k != std::string::npos) {
            name.erase(k, 10);
        }

        outputStream << "{";
        outputStream << "\"cat\":\"function\",";
        outputStream << "\"dur\":" << (result.end - result.start) << ',';
        outputStream << "\"name\":\"" << name << "\",";
        outputStream << "\"ph\":\"X\",";
        outputStream << "\"pid\":0,";
        outputStream << "\"tid\":" << result.threadID << ",";
        outputStream << "\"ts\":" << result.start;
        outputStream << "}";

        outputStream.flush();
    }
}

void Benchmark::WriteHeader() {
    outputStream << "{\"otherData\": {},\"traceEvents\":[";
    outputStream.flush();
}

void Benchmark::WriteFooter() {
    outputStream << "]}";
    outputStream.flush();
}

Benchmark* Benchmark::Get() {
    static Benchmark* instance = new Benchmark;
    return instance;
}