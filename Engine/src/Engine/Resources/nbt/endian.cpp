#include "enpch.hpp"
#include "Engine/Resources/nbt/endian.hpp"
#include "Engine/Core/GameMath.hpp"

static_assert(CHAR_BIT == 8, "Assuming that a byte has 8 bits");
static_assert(sizeof(float) == 4, "Assuming that a float is 4 byte long");
static_assert(sizeof(double) == 8, "Assuming that a double is 8 byte long");

namespace endian
{

    namespace //anonymous
    {
        void put_int_to_float(float& f, uint32_t i) {
            memcpy(&f, &i, 4);
        }

        uint32_t put_float_to_int(float f) {
            uint32_t ret;
            memcpy(&ret, &f, 4);
            return ret;
        }

        void put_int_to_double(double& d, uint64_t i) {
            memcpy(&d, &i, 8);
        }

        uint64_t put_double_to_int(double f) {
            uint64_t ret;
            memcpy(&ret, &f, 8);
            return ret;
        }
    }

    //------------------------------------------------------------------------------

    void read_little(std::istream& is, uint8_t& x) {
        is.get(reinterpret_cast<char&>(x));
    }

    void read_little(std::istream& is, uint16_t& x) {
        uint8_t tmp[2];
        is.read(reinterpret_cast<char*>(tmp), 2);
        x = uint16_t(tmp[0])
            | (uint16_t(tmp[1]) << 8);
    }

    void read_little(std::istream& is, uint32_t& x) {
        uint8_t tmp[4];
        is.read(reinterpret_cast<char*>(tmp), 4);
        x = uint32_t(tmp[0])
            | (uint32_t(tmp[1]) << 8)
            | (uint32_t(tmp[2]) << 16)
            | (uint32_t(tmp[3]) << 24);
    }

    void read_little(std::istream& is, uint64_t& x) {
        uint8_t tmp[8];
        is.read(reinterpret_cast<char*>(tmp), 8);
        x = uint64_t(tmp[0])
            | (uint64_t(tmp[1]) << 8)
            | (uint64_t(tmp[2]) << 16)
            | (uint64_t(tmp[3]) << 24)
            | (uint64_t(tmp[4]) << 32)
            | (uint64_t(tmp[5]) << 40)
            | (uint64_t(tmp[6]) << 48)
            | (uint64_t(tmp[7]) << 56);
    }

    void read_little(std::istream& is, int8_t& x) { read_little(is, reinterpret_cast<uint8_t&>(x)); }
    void read_little(std::istream& is, int16_t& x) { read_little(is, reinterpret_cast<uint16_t&>(x)); }
    void read_little(std::istream& is, int32_t& x) { read_little(is, reinterpret_cast<uint32_t&>(x)); }
    void read_little(std::istream& is, int64_t& x) { read_little(is, reinterpret_cast<uint64_t&>(x)); }

    void read_little(std::istream& is, float& x) {
        uint32_t tmp;
        read_little(is, tmp);
        put_int_to_float(x, tmp);
    }

    void read_little(std::istream& is, double& x) {
        uint64_t tmp;
        read_little(is, tmp);
        put_int_to_double(x, tmp);
    }

    void read_little(std::istream& is, math::vec2& x) {
        uint32_t t1, t2;
        read_little(is, t1);
        read_little(is, t2);
        put_int_to_float(x.x, t1);
        put_int_to_float(x.y, t2);
    }

    void read_little(std::istream& is, math::vec3& x) {
        uint32_t t1, t2, t3;
        read_little(is, t1);
        read_little(is, t2);
        read_little(is, t3);
        put_int_to_float(x.x, t1);
        put_int_to_float(x.y, t2);
        put_int_to_float(x.z, t3);
    }

    void read_little(std::istream& is, math::vec4& x) {
        uint32_t t1, t2, t3, t4;
        read_little(is, t1);
        read_little(is, t2);
        read_little(is, t3);
        read_little(is, t4);
        put_int_to_float(x.x, t1);
        put_int_to_float(x.y, t2);
        put_int_to_float(x.z, t3);
        put_int_to_float(x.w, t4);
    }

    void read_little(std::istream& is, math::mat2& x) {
        uint32_t t11, t12, t21, t22;
        read_little(is, t11);
        read_little(is, t12);
        read_little(is, t21);
        read_little(is, t22);
        put_int_to_float(x._11, t11);
        put_int_to_float(x._12, t12);
        put_int_to_float(x._21, t21);
        put_int_to_float(x._22, t22);
    }

    void read_little(std::istream& is, math::mat3& x) {
        uint32_t t11, t12, t13, t21, t22, t23, t31, t32, t33;
        read_little(is, t11);
        read_little(is, t12);
        read_little(is, t13);
        read_little(is, t21);
        read_little(is, t22);
        read_little(is, t23);
        read_little(is, t31);
        read_little(is, t32);
        read_little(is, t33);
        put_int_to_float(x._11, t11);
        put_int_to_float(x._12, t12);
        put_int_to_float(x._13, t13);
        put_int_to_float(x._21, t21);
        put_int_to_float(x._22, t22);
        put_int_to_float(x._23, t23);
        put_int_to_float(x._31, t31);
        put_int_to_float(x._32, t32);
        put_int_to_float(x._33, t33);
    }

    void read_little(std::istream& is, math::mat4& x) {
        uint32_t t11, t12, t13, t14, t21, t22, t23, t24, t31, t32, t33, t34, t41, t42, t43, t44;
        read_little(is, t11);
        read_little(is, t12);
        read_little(is, t13);
        read_little(is, t14);
        read_little(is, t21);
        read_little(is, t22);
        read_little(is, t23);
        read_little(is, t24);
        read_little(is, t31);
        read_little(is, t32);
        read_little(is, t33);
        read_little(is, t34);
        read_little(is, t41);
        read_little(is, t42);
        read_little(is, t43);
        read_little(is, t44);
        put_int_to_float(x._11, t11);
        put_int_to_float(x._12, t12);
        put_int_to_float(x._13, t13);
        put_int_to_float(x._14, t14);
        put_int_to_float(x._21, t21);
        put_int_to_float(x._22, t22);
        put_int_to_float(x._23, t23);
        put_int_to_float(x._24, t24);
        put_int_to_float(x._31, t31);
        put_int_to_float(x._32, t32);
        put_int_to_float(x._33, t33);
        put_int_to_float(x._34, t34);
        put_int_to_float(x._41, t41);
        put_int_to_float(x._42, t42);
        put_int_to_float(x._43, t43);
        put_int_to_float(x._44, t44);
    }

    //------------------------------------------------------------------------------

    void read_big(std::istream& is, uint8_t& x) {
        is.read(reinterpret_cast<char*>(&x), 1);
    }

    void read_big(std::istream& is, uint16_t& x) {
        uint8_t tmp[2];
        is.read(reinterpret_cast<char*>(tmp), 2);
        x = uint16_t(tmp[1])
            | (uint16_t(tmp[0]) << 8);
    }

    void read_big(std::istream& is, uint32_t& x) {
        uint8_t tmp[4];
        is.read(reinterpret_cast<char*>(tmp), 4);
        x = uint32_t(tmp[3])
            | (uint32_t(tmp[2]) << 8)
            | (uint32_t(tmp[1]) << 16)
            | (uint32_t(tmp[0]) << 24);
    }

    void read_big(std::istream& is, uint64_t& x) {
        uint8_t tmp[8];
        is.read(reinterpret_cast<char*>(tmp), 8);
        x = uint64_t(tmp[7])
            | (uint64_t(tmp[6]) << 8)
            | (uint64_t(tmp[5]) << 16)
            | (uint64_t(tmp[4]) << 24)
            | (uint64_t(tmp[3]) << 32)
            | (uint64_t(tmp[2]) << 40)
            | (uint64_t(tmp[1]) << 48)
            | (uint64_t(tmp[0]) << 56);
    }

    void read_big(std::istream& is, int8_t& x) { read_big(is, reinterpret_cast<uint8_t&>(x)); }
    void read_big(std::istream& is, int16_t& x) { read_big(is, reinterpret_cast<uint16_t&>(x)); }
    void read_big(std::istream& is, int32_t& x) { read_big(is, reinterpret_cast<uint32_t&>(x)); }
    void read_big(std::istream& is, int64_t& x) { read_big(is, reinterpret_cast<uint64_t&>(x)); }

    void read_big(std::istream& is, float& x) {
        uint32_t tmp;
        read_big(is, tmp);
        put_int_to_float(x, tmp);
    }

    void read_big(std::istream& is, double& x) {
        uint64_t tmp;
        read_big(is, tmp);
        put_int_to_double(x, tmp);
    }

    void read_big(std::istream& is, math::vec2& x) {
        uint32_t t1, t2;
        read_big(is, t1);
        read_big(is, t2);
        put_int_to_float(x.x, t1);
        put_int_to_float(x.y, t2);
    }

    void read_big(std::istream& is, math::vec3& x) {
        uint32_t t1, t2, t3;
        read_big(is, t1);
        read_big(is, t2);
        read_big(is, t3);
        put_int_to_float(x.x, t1);
        put_int_to_float(x.y, t2);
        put_int_to_float(x.z, t3);
    }

    void read_big(std::istream& is, math::vec4& x) {
        uint32_t t1, t2, t3, t4;
        read_big(is, t1);
        read_big(is, t2);
        read_big(is, t3);
        read_big(is, t4);
        put_int_to_float(x.x, t1);
        put_int_to_float(x.y, t2);
        put_int_to_float(x.z, t3);
        put_int_to_float(x.w, t4);
    }

    void read_big(std::istream& is, math::mat2& x) {
        uint32_t t11, t12, t21, t22;
        read_big(is, t11);
        read_big(is, t12);
        read_big(is, t21);
        read_big(is, t22);
        put_int_to_float(x._11, t11);
        put_int_to_float(x._12, t12);
        put_int_to_float(x._21, t21);
        put_int_to_float(x._22, t22);
    }

    void read_big(std::istream& is, math::mat3& x) {
        uint32_t t11, t12, t13, t21, t22, t23, t31, t32, t33;
        read_big(is, t11);
        read_big(is, t12);
        read_big(is, t13);
        read_big(is, t21);
        read_big(is, t22);
        read_big(is, t23);
        read_big(is, t31);
        read_big(is, t32);
        read_big(is, t33);
        put_int_to_float(x._11, t11);
        put_int_to_float(x._12, t12);
        put_int_to_float(x._13, t13);
        put_int_to_float(x._21, t21);
        put_int_to_float(x._22, t22);
        put_int_to_float(x._23, t23);
        put_int_to_float(x._31, t31);
        put_int_to_float(x._32, t32);
        put_int_to_float(x._33, t33);
    }

    void read_big(std::istream& is, math::mat4& x) {
        uint32_t t11, t12, t13, t14, t21, t22, t23, t24, t31, t32, t33, t34, t41, t42, t43, t44;
        read_big(is, t11);
        read_big(is, t12);
        read_big(is, t13);
        read_big(is, t14);
        read_big(is, t21);
        read_big(is, t22);
        read_big(is, t23);
        read_big(is, t24);
        read_big(is, t31);
        read_big(is, t32);
        read_big(is, t33);
        read_big(is, t34);
        read_big(is, t41);
        read_big(is, t42);
        read_big(is, t43);
        read_big(is, t44);
        put_int_to_float(x._11, t11);
        put_int_to_float(x._12, t12);
        put_int_to_float(x._13, t13);
        put_int_to_float(x._14, t14);
        put_int_to_float(x._21, t21);
        put_int_to_float(x._22, t22);
        put_int_to_float(x._23, t23);
        put_int_to_float(x._24, t24);
        put_int_to_float(x._31, t31);
        put_int_to_float(x._32, t32);
        put_int_to_float(x._33, t33);
        put_int_to_float(x._34, t34);
        put_int_to_float(x._41, t41);
        put_int_to_float(x._42, t42);
        put_int_to_float(x._43, t43);
        put_int_to_float(x._44, t44);
    }

    //------------------------------------------------------------------------------

    void write_little(std::ostream& os, uint8_t x) {
        os.put(x);
    }

    void write_little(std::ostream& os, uint16_t x) {
        uint8_t tmp[2]{
            uint8_t(x),
            uint8_t(x >> 8) };
        os.write(reinterpret_cast<const char*>(tmp), 2);
    }

    void write_little(std::ostream& os, uint32_t x) {
        uint8_t tmp[4]{
            uint8_t(x),
            uint8_t(x >> 8),
            uint8_t(x >> 16),
            uint8_t(x >> 24) };
        os.write(reinterpret_cast<const char*>(tmp), 4);
    }

    void write_little(std::ostream& os, uint64_t x) {
        uint8_t tmp[8]{
            uint8_t(x),
            uint8_t(x >> 8),
            uint8_t(x >> 16),
            uint8_t(x >> 24),
            uint8_t(x >> 32),
            uint8_t(x >> 40),
            uint8_t(x >> 48),
            uint8_t(x >> 56) };
        os.write(reinterpret_cast<const char*>(tmp), 8);
    }

    void write_little(std::ostream& os, int8_t  x) { write_little(os, static_cast<uint8_t>(x)); }
    void write_little(std::ostream& os, int16_t x) { write_little(os, static_cast<uint16_t>(x)); }
    void write_little(std::ostream& os, int32_t x) { write_little(os, static_cast<uint32_t>(x)); }
    void write_little(std::ostream& os, int64_t x) { write_little(os, static_cast<uint64_t>(x)); }

    void write_little(std::ostream& os, float x) {
        write_little(os, put_float_to_int(x));
    }

    void write_little(std::ostream& os, double x) {
        write_little(os, put_double_to_int(x));
    }

    void write_little(std::ostream& os, math::vec2 x) {
        write_little(os, put_float_to_int(x.x));
        write_little(os, put_float_to_int(x.y));
    }

    void write_little(std::ostream& os, math::vec3 x) {
        write_little(os, put_float_to_int(x.x));
        write_little(os, put_float_to_int(x.y));
        write_little(os, put_float_to_int(x.z));
    }

    void write_little(std::ostream& os, math::vec4 x) {
        write_little(os, put_float_to_int(x.x));
        write_little(os, put_float_to_int(x.y));
        write_little(os, put_float_to_int(x.z));
        write_little(os, put_float_to_int(x.w));
    }

    void write_little(std::ostream& os, math::mat2 x) {
        write_little(os, put_float_to_int(x._11));
        write_little(os, put_float_to_int(x._12));

        write_little(os, put_float_to_int(x._21));
        write_little(os, put_float_to_int(x._22));
    }

    void write_little(std::ostream& os, math::mat3 x) {
        write_little(os, put_float_to_int(x._11));
        write_little(os, put_float_to_int(x._12));
        write_little(os, put_float_to_int(x._13));

        write_little(os, put_float_to_int(x._21));
        write_little(os, put_float_to_int(x._22));
        write_little(os, put_float_to_int(x._23));

        write_little(os, put_float_to_int(x._31));
        write_little(os, put_float_to_int(x._32));
        write_little(os, put_float_to_int(x._33));
    }

    void write_little(std::ostream& os, math::mat4 x) {
        write_little(os, put_float_to_int(x._11));
        write_little(os, put_float_to_int(x._12));
        write_little(os, put_float_to_int(x._13));
        write_little(os, put_float_to_int(x._14));

        write_little(os, put_float_to_int(x._21));
        write_little(os, put_float_to_int(x._22));
        write_little(os, put_float_to_int(x._23));
        write_little(os, put_float_to_int(x._24));

        write_little(os, put_float_to_int(x._31));
        write_little(os, put_float_to_int(x._32));
        write_little(os, put_float_to_int(x._33));
        write_little(os, put_float_to_int(x._34));

        write_little(os, put_float_to_int(x._41));
        write_little(os, put_float_to_int(x._42));
        write_little(os, put_float_to_int(x._43));
        write_little(os, put_float_to_int(x._44));
    }

    //------------------------------------------------------------------------------

    void write_big(std::ostream& os, uint8_t x) {
        os.put(x);
    }

    void write_big(std::ostream& os, uint16_t x) {
        uint8_t tmp[2]{
            uint8_t(x >> 8),
            uint8_t(x) };
        os.write(reinterpret_cast<const char*>(tmp), 2);
    }

    void write_big(std::ostream& os, uint32_t x) {
        uint8_t tmp[4]{
            uint8_t(x >> 24),
            uint8_t(x >> 16),
            uint8_t(x >> 8),
            uint8_t(x) };
        os.write(reinterpret_cast<const char*>(tmp), 4);
    }

    void write_big(std::ostream& os, uint64_t x) {
        uint8_t tmp[8]{
            uint8_t(x >> 56),
            uint8_t(x >> 48),
            uint8_t(x >> 40),
            uint8_t(x >> 32),
            uint8_t(x >> 24),
            uint8_t(x >> 16),
            uint8_t(x >> 8),
            uint8_t(x) };
        os.write(reinterpret_cast<const char*>(tmp), 8);
    }

    void write_big(std::ostream& os, int8_t  x) { write_big(os, static_cast<uint8_t>(x)); }
    void write_big(std::ostream& os, int16_t x) { write_big(os, static_cast<uint16_t>(x)); }
    void write_big(std::ostream& os, int32_t x) { write_big(os, static_cast<uint32_t>(x)); }
    void write_big(std::ostream& os, int64_t x) { write_big(os, static_cast<uint64_t>(x)); }

    void write_big(std::ostream& os, float x) {
        write_big(os, put_float_to_int(x));
    }

    void write_big(std::ostream& os, double x) {
        write_big(os, put_double_to_int(x));
    }

    void write_big(std::ostream& os, math::vec2 x) {
        write_big(os, put_float_to_int(x.x));
        write_big(os, put_float_to_int(x.y));
    }

    void write_big(std::ostream& os, math::vec3 x) {
        write_big(os, put_float_to_int(x.x));
        write_big(os, put_float_to_int(x.y));
        write_big(os, put_float_to_int(x.z));
    }

    void write_big(std::ostream& os, math::vec4 x) {
        write_big(os, put_float_to_int(x.x));
        write_big(os, put_float_to_int(x.y));
        write_big(os, put_float_to_int(x.z));
        write_big(os, put_float_to_int(x.w));
    }

    void write_big(std::ostream& os, math::mat2 x) {
        write_big(os, put_float_to_int(x._11));
        write_big(os, put_float_to_int(x._12));

        write_big(os, put_float_to_int(x._21));
        write_big(os, put_float_to_int(x._22));
    }

    void write_big(std::ostream& os, math::mat3 x) {
        write_big(os, put_float_to_int(x._11));
        write_big(os, put_float_to_int(x._12));
        write_big(os, put_float_to_int(x._13));

        write_big(os, put_float_to_int(x._21));
        write_big(os, put_float_to_int(x._22));
        write_big(os, put_float_to_int(x._23));

        write_big(os, put_float_to_int(x._31));
        write_big(os, put_float_to_int(x._32));
        write_big(os, put_float_to_int(x._33));
    }

    void write_big(std::ostream& os, math::mat4 x) {
        write_big(os, put_float_to_int(x._11));
        write_big(os, put_float_to_int(x._12));
        write_big(os, put_float_to_int(x._13));
        write_big(os, put_float_to_int(x._14));
              
        write_big(os, put_float_to_int(x._21));
        write_big(os, put_float_to_int(x._22));
        write_big(os, put_float_to_int(x._23));
        write_big(os, put_float_to_int(x._24));
              
        write_big(os, put_float_to_int(x._31));
        write_big(os, put_float_to_int(x._32));
        write_big(os, put_float_to_int(x._33));
        write_big(os, put_float_to_int(x._34));

        write_big(os, put_float_to_int(x._41));
        write_big(os, put_float_to_int(x._42));
        write_big(os, put_float_to_int(x._43));
        write_big(os, put_float_to_int(x._44));
    }

}
