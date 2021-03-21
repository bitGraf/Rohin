#pragma once

#include "Engine/Core/Logger.hpp"
#include "Engine/Core/Platform.hpp"
#include "Engine/Core/DataTypes.hpp"
#include <memory>

/* Assertion macros */

#ifdef NO_ASSERTS
    #define ENGINE_LOG_ASSERT(x, ...) 
    #define LOG_ASSERT(x, ...) 
#else
    #define ENGINE_LOG_ASSERT(x, ...) {if(!(x)) {ENGINE_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
    #define LOG_ASSERT(x, ...)        {if(!(x)) {LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#endif

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

//#define NO_DEBUG_OSTR
#ifdef NO_DEBUG_OSTR
#define DEBUG_OSTR_IMPL(type) 
#define DEBUG_OSTR(obj) 0
#else
#define DEBUG_OSTR_IMPL(type) friend std::ostream& operator<<(std::ostream& os, const type& c){return os << #type;}
#define DEBUG_OSTR(obj) obj
#endif

namespace Engine {
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;
}