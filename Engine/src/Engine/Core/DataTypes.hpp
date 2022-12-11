#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <cstdint>
#include <entt/fwd.hpp>
#include <laml/data_types.hpp>

namespace rh {
    /* Hashed String ID */
    typedef u32 stringID;

    using GameObject_type = entt::entity;

    /* Text alignmnet */
    enum TextAlignment {
        ALIGN_TOP_LEFT = 0,
        ALIGN_MID_LEFT,
        ALIGN_BOT_LEFT,
        ALIGN_TOP_MID,
        ALIGN_MID_MID,
        ALIGN_BOT_MID,
        ALIGN_TOP_RIGHT,
        ALIGN_MID_RIGHT,
        ALIGN_BOT_RIGHT
    };
}

/* Ensure base data types are the correct size */
static_assert(sizeof(rh::f32) == 4, "Float32 not 32-bits");
static_assert(sizeof(rh::f64) == 8, "Float64 not 64-bits");

static_assert(sizeof(rh::u8)  == 1, "UInt8  not 8-bits");
static_assert(sizeof(rh::u16) == 2, "UInt16 not 16-bits");
static_assert(sizeof(rh::u32) == 4, "UInt32 not 32-bits");
static_assert(sizeof(rh::u64) == 8, "UInt64 not 64-bits");

static_assert(sizeof(rh::s8)  == 1, "SInt8  not 8-bits");
static_assert(sizeof(rh::s16) == 2, "SInt16 not 16-bits");
static_assert(sizeof(rh::s32) == 4, "SInt32 not 32-bits");
static_assert(sizeof(rh::s64) == 8, "SInt64 not 64-bits");

#endif
