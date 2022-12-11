#pragma once

#include <stdint.h>
#include <string>

namespace rh::nbt {

	typedef std::int8_t		nbt_byte;
	typedef std::int16_t	nbt_short;
	typedef std::int32_t	nbt_int;
	typedef std::int64_t	nbt_long;
	typedef float			nbt_float;
	typedef double			nbt_double;
	typedef std::string		nbt_string;
    //typedef laml::Vec3      nbt_vec3;

    enum class tag_type : nbt_byte
    {
        End = 0,
        Byte = 1,
        Short = 2,
        Int = 3,
        Long = 4,
        Float = 5,
        Double = 6,
        Byte_Array = 7,
        String = 8,
        List = 9,
        Compound = 10,
        Int_Array = 11,
        Long_Array = 12,
        Vector2 = 13,
        Vector3 = 14,
        Vector4 = 15,
        Matrix2 = 16,
        Matrix3 = 17,
        Matrix4 = 18,
        Null = -1   ///< Used to denote empty value
    };
}