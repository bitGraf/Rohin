#include <enpch.hpp>
#include "Engine/Core/Logger.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace rh {
    std::shared_ptr<spdlog::logger> Logger::_Engine_Logger;
    std::shared_ptr<spdlog::logger> Logger::_Game_Logger;

    void Logger::Init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        _Engine_Logger = spdlog::stdout_color_mt("ENGINE");
        _Engine_Logger->set_level(spdlog::level::trace);

        _Game_Logger = spdlog::stdout_color_mt("GAME");
        _Game_Logger->set_level(spdlog::level::trace);
    }

    void PrintConfiguration() {
#ifdef RH_RELEASE
        ENGINE_LOG_CRITICAL("Running in RELEASE mode");
#endif
#ifdef RH_DEBUG
        ENGINE_LOG_CRITICAL("Running in DEBUG mode");
#endif
#ifdef RH_DIST
        ENGINE_LOG_CRITICAL("Running in DIST mode");
#endif
    }
}