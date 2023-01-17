#pragma once

#include "Engine/Defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// Disable debug and trace logging for release builds.
#if RH_RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
};

RHAPI bool32 InitLogging();
RHAPI void ShutdownLogging();

RHAPI void LogOutput(log_level Level, const char* Message, ...);

// Logs a fatal-level message.
#define RH_FATAL(Message, ...) LogOutput(LOG_LEVEL_FATAL, Message, ##__VA_ARGS__);

#ifndef RH_ERROR
// Logs an error-level message.
#define RH_ERROR(Message, ...) LogOutput(LOG_LEVEL_ERROR, Message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
// Logs a warning-level message.
#define RH_WARN(Message, ...) LogOutput(LOG_LEVEL_WARN, Message, ##__VA_ARGS__);
#else
// Does nothing when LOG_WARN_ENABLED != 1
#define RH_WARN(Message, ...)
#endif

#if LOG_INFO_ENABLED == 1
// Logs a info-level message.
#define RH_INFO(Message, ...) LogOutput(LOG_LEVEL_INFO, Message, ##__VA_ARGS__);
#else
// Does nothing when LOG_INFO_ENABLED != 1
#define RH_INFO(Message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
// Logs a debug-level message.
#define RH_DEBUG(Message, ...) LogOutput(LOG_LEVEL_DEBUG, Message, ##__VA_ARGS__);
#else
// Does nothing when LOG_DEBUG_ENABLED != 1
#define RH_DEBUG(Message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
// Logs a trace-level message.
#define RH_TRACE(Message, ...) LogOutput(LOG_LEVEL_TRACE, Message, ##__VA_ARGS__);
#else
// Does nothing when LOG_TRACE_ENABLED != 1
#define RH_TRACE(Message, ...)
#endif