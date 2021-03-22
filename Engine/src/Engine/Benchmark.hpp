#ifndef BENCHMARK_HPP_
#define BENCHMARK_HPP_

#include <thread>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <string>
#include <iostream>

#define BENCHMARK_ENABLE 1
#if BENCHMARK_ENABLE
#define BENCHMARK_SCOPE(name) BenchmarkTimer timer##__LINE__(name)
#define BENCHMARK_FUNCTION() BENCHMARK_SCOPE(__FUNCTION__)
#define BENCHMARK_START_SESSION(name,filepath) Benchmark::Get()->BeginSession(name,filepath);
#define BENCHMARK_END_SESSION() Benchmark::Get()->EndSession();
#else
#define BENCHMARK_SCOPE(name) 
#define BENCHMARK_FUNCTION()
#define BENCHMARK_START_SESSION(name,filepath) 
#define BENCHMARK_END_SESSION()
#endif

class ScopeTimer {
public:
    ScopeTimer(const char* _name);
    ~ScopeTimer();

    void Stop();

private:
    const char* name;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    bool done;
};

class BenchmarkTimer {
public:
    BenchmarkTimer(const char* _name);
    ~BenchmarkTimer();

    void Stop();

private:
    const char* name;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    bool done;
};


struct ProfileResult {
    std::string name;
    long long start, end;
    uint32_t threadID;
};

struct BenchmarkSession {
    std::string name;
};

class Benchmark {
public:
    Benchmark();

    void BeginSession(const std::string& name, const std::string& filepath = "results.json");
    void EndSession();
    void WriteProfile(const ProfileResult& result);

    void WriteHeader();
    void WriteFooter();

    static Benchmark* Get();

private:
    BenchmarkSession * currentSession;
    std::ofstream outputStream;
    int numProfiles;
};

#endif