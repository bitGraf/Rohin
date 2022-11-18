#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "Engine/Core/base.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Engine {
    class Logger {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return _Engine_Logger; }
        inline static std::shared_ptr<spdlog::logger>& GetGameLogger() { return _Game_Logger; }

    private:
        static std::shared_ptr<spdlog::logger> _Engine_Logger;
        static std::shared_ptr<spdlog::logger> _Game_Logger;
    };

    void PrintConfiguration();
}

/* Log macros */
//#define NO_LOGGING
#ifdef NO_LOGGING
    #define ENGINE_LOG_DEBUG(...)
    #define ENGINE_LOG_TRACE(...)
    #define ENGINE_LOG_INFO(...)
    #define ENGINE_LOG_WARN(...)
    #define ENGINE_LOG_ERROR(...)
    #define ENGINE_LOG_FATAL(...)

    #define LOG_DEBUG(...)
    #define LOG_TRACE(...)
    #define LOG_INFO(...)
    #define LOG_WARN(...)
    #define LOG_ERROR(...)
    #define LOG_FATAL(...)
#else
    #define ENGINE_LOG_DEBUG(...)    ::Engine::Logger::GetEngineLogger()->debug(__VA_ARGS__)
    #define ENGINE_LOG_TRACE(...)    ::Engine::Logger::GetEngineLogger()->trace(__VA_ARGS__)
    #define ENGINE_LOG_INFO(...)     ::Engine::Logger::GetEngineLogger()->info(__VA_ARGS__)
    #define ENGINE_LOG_WARN(...)     ::Engine::Logger::GetEngineLogger()->warn(__VA_ARGS__)
    #define ENGINE_LOG_ERROR(...)    ::Engine::Logger::GetEngineLogger()->error(__VA_ARGS__)
    #define ENGINE_LOG_CRITICAL(...) ::Engine::Logger::GetEngineLogger()->critical(__VA_ARGS__)

    #define LOG_DEBUG(...)    ::Engine::Logger::GetGameLogger()->debug(__VA_ARGS__)
    #define LOG_TRACE(...)    ::Engine::Logger::GetGameLogger()->trace(__VA_ARGS__)
    #define LOG_INFO(...)     ::Engine::Logger::GetGameLogger()->info(__VA_ARGS__)
    #define LOG_WARN(...)     ::Engine::Logger::GetGameLogger()->warn(__VA_ARGS__)
    #define LOG_ERROR(...)    ::Engine::Logger::GetGameLogger()->error(__VA_ARGS__)
    #define LOG_CRITICAL(...) ::Engine::Logger::GetGameLogger()->critical(__VA_ARGS__)
#endif

#endif