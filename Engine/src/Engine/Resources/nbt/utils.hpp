#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Resources/nbt/data.hpp"

namespace rh::nbt {

    namespace utils {
        ///Meta-struct that holds the tag_type value for a specific primitive type
        template<class T> struct get_primitive_type
        {
            static_assert(sizeof(T) != sizeof(T), "Invalid type paramter for tag_primitive, can only use types that NBT uses");
        };

        template<> struct get_primitive_type<nbt_byte> : public std::integral_constant<tag_type, tag_type::Byte> {};
        template<> struct get_primitive_type<nbt_short> : public std::integral_constant<tag_type, tag_type::Short> {};
        template<> struct get_primitive_type<nbt_int> : public std::integral_constant<tag_type, tag_type::Int> {};
        template<> struct get_primitive_type<nbt_long> : public std::integral_constant<tag_type, tag_type::Long> {};
        template<> struct get_primitive_type<nbt_float> : public std::integral_constant<tag_type, tag_type::Float> {};
        template<> struct get_primitive_type<nbt_double> : public std::integral_constant<tag_type, tag_type::Double> {};

        template<> struct get_primitive_type<laml::Vec2> : public std::integral_constant<tag_type, tag_type::Vector2> {};
        template<> struct get_primitive_type<laml::Vec3> : public std::integral_constant<tag_type, tag_type::Vector3> {};
        template<> struct get_primitive_type<laml::Vec4> : public std::integral_constant<tag_type, tag_type::Vector4> {};
        template<> struct get_primitive_type<laml::Mat2> : public std::integral_constant<tag_type, tag_type::Matrix2> {};
        template<> struct get_primitive_type<laml::Mat3> : public std::integral_constant<tag_type, tag_type::Matrix3> {};
        template<> struct get_primitive_type<laml::Mat4> : public std::integral_constant<tag_type, tag_type::Matrix4> {};


        ///Meta-struct that holds the tag_type value for a specific array type
        template<class T> struct get_array_type
        {
            static_assert(sizeof(T) != sizeof(T), "Invalid type parameter for tag_array, can only use byte or int");
        };

        template<> struct get_array_type<nbt_byte> : public std::integral_constant<tag_type, tag_type::Byte_Array> {};
        template<> struct get_array_type<nbt_int> : public std::integral_constant<tag_type, tag_type::Int_Array> {};
        template<> struct get_array_type<nbt_long> : public std::integral_constant<tag_type, tag_type::Long_Array> {};
    }
}