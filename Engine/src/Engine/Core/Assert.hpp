#include "Engine/Core/Logger.hpp"
/* Assertion macros */

#ifdef NO_ASSERTS
#define ENGINE_LOG_ASSERT(x, ...) 
#define LOG_ASSERT(x, ...) 
#else
#define ENGINE_LOG_ASSERT(x, ...) {if(!(x)) {ENGINE_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define LOG_ASSERT(x, ...)        {if(!(x)) {LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#endif