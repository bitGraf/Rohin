#ifndef BASE_H
#define BASE_H

#ifndef RH_TYPES_DEFINED
#define RH_TYPES_DEFINED

#include <stdint.h>

#define local_persist   static
#define global_variable static
#define internal_func   static

#define Pi32 3.14159265359f

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

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#endif

#if ROHIN_SLOW
#define Assert(Expression) if (!(Expression)) { *(int *)0 = 0; }
#define ENGINE_LOG_ASSERT(Expression, msg) Assert(Expression)
#else
#define Assert(Expression)
#define ENGINE_LOG_ASSERT(Expression, msg)
#endif

#include <laml/laml.hpp>

#endif