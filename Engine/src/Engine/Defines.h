#pragma once

#include <stdint.h>
#include <cstddef>

#ifndef RH_TYPES
#define RH_TYPES
typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float  real32;
typedef double real64;

typedef size_t memory_index;
#endif

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define local_persist   static
#define global_variable static
#define internal_func   static


// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) 
#define RH_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#define RH_PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define RH_PLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
// Catch anything not caught by the above.
#define RH_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#define RH_PLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#define RH_PLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#define RH_PLATFORM_IOS 1
#define RH_PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define RH_PLATFORM_IOS 1
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform!"
#endif

// .dll export api
#ifdef RH_EXPORT
// Exports
#ifdef _MSC_VER
#define RHAPI __declspec(dllexport)
#else
#define RHAPI __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define RHAPI __declspec(dllimport)
#else
#define RHAPI
#endif
#endif