#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <cstdint>

/* Floating point types */
typedef float   f32;
typedef double  f64;

/* Unsinged integers */
typedef std::uint8_t    u8;     // [0, 255]
typedef std::uint16_t   u16;    // [0, 65,535]
typedef std::uint32_t   u32;    // [0, 4,294,967,295]
typedef std::uint64_t   u64;    // [0, 18,446,744,073,709,551,615]

/* Singed integers */
typedef std::int8_t     s8;     // [-128, 127]
typedef std::int16_t    s16;    // [-32768, 32767]
typedef std::int32_t    s32;    // [-2,147,483,648, 2,147,483,647]
typedef std::int64_t    s64;    // [-9,223,372,036,854,775,808, 9,223,372,036,854,775,807]

/* Hashed String ID */
typedef u32 stringID;

/* TriangleMesh Index type */
typedef u16                     index_t;

/* Ensure base data types are the correct size */
static_assert(sizeof(f32) == 4, "Float32 not 32-bits");
static_assert(sizeof(f64) == 8, "Float64 not 64-bits");

static_assert(sizeof(u8)  == 1, "UInt8  not 8-bits");
static_assert(sizeof(u16) == 2, "UInt16 not 16-bits");
static_assert(sizeof(u32) == 4, "UInt32 not 32-bits");
static_assert(sizeof(u64) == 8, "UInt64 not 64-bits");

static_assert(sizeof(s8)  == 1, "SInt8  not 8-bits");
static_assert(sizeof(s16) == 2, "SInt16 not 16-bits");
static_assert(sizeof(s32) == 4, "SInt32 not 32-bits");
static_assert(sizeof(s64) == 8, "SInt64 not 64-bits");

#endif
