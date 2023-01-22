#pragma once

#include "Engine/defines.h"

// Disable assertions by commenting out the below line.
#define RH_ASSERTIONS_ENABLED

#ifdef RH_ASSERTIONS_ENABLED
    #if _MSC_VER
        #include <intrin.h>
        #define debugBreak() __debugbreak()
    #else
        #define debugBreak() __builtin_trap()
    #endif

    RHAPI void ReportAssertionFailure(const char* expression, const char* message, const char* file, int32 line);

    #define Assert(expr)                                                 \
        {                                                                \
            if (expr) {                                                  \
            } else {                                                     \
                ReportAssertionFailure(#expr, "", __FILE__, __LINE__);   \
                debugBreak();                                            \
            }                                                            \
        }

    #define AssertMsg(expr, message)                                         \
        {                                                                     \
            if (expr) {                                                       \
            } else {                                                          \
                ReportAssertionFailure(#expr, message, __FILE__, __LINE__);   \
                debugBreak();                                                 \
            }                                                                 \
        }

    #ifdef _DEBUG
        #define AssertDebug(expr)                                           \
            {                                                                \
                if (expr) {                                                  \
                } else {                                                     \
                    ReportAssertionFailure(#expr, "", __FILE__, __LINE__);   \
                    debugBreak();                                            \
                }                                                            \
            }
    #else
        #define Assert_debug(expr)  // Does nothing at all
    #endif

#else
    #define Assert(expr)               // Does nothing at all
    #define AssertMsg(expr, message)  // Does nothing at all
    #define AssertDebug(expr)         // Does nothing at all
#endif