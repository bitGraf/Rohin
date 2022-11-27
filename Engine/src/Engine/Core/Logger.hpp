#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace rh {
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
    #define ENGINE_LOG_DEBUG(...)    ::rh::Logger::GetEngineLogger()->debug(__VA_ARGS__)
    #define ENGINE_LOG_TRACE(...)    ::rh::Logger::GetEngineLogger()->trace(__VA_ARGS__)
    #define ENGINE_LOG_INFO(...)     ::rh::Logger::GetEngineLogger()->info(__VA_ARGS__)
    #define ENGINE_LOG_WARN(...)     ::rh::Logger::GetEngineLogger()->warn(__VA_ARGS__)
    #define ENGINE_LOG_ERROR(...)    ::rh::Logger::GetEngineLogger()->error(__VA_ARGS__)
    #define ENGINE_LOG_CRITICAL(...) ::rh::Logger::GetEngineLogger()->critical(__VA_ARGS__)

    #define LOG_DEBUG(...)    ::rh::Logger::GetGameLogger()->debug(__VA_ARGS__)
    #define LOG_TRACE(...)    ::rh::Logger::GetGameLogger()->trace(__VA_ARGS__)
    #define LOG_INFO(...)     ::rh::Logger::GetGameLogger()->info(__VA_ARGS__)
    #define LOG_WARN(...)     ::rh::Logger::GetGameLogger()->warn(__VA_ARGS__)
    #define LOG_ERROR(...)    ::rh::Logger::GetGameLogger()->error(__VA_ARGS__)
    #define LOG_CRITICAL(...) ::rh::Logger::GetGameLogger()->critical(__VA_ARGS__)
#endif

#endif