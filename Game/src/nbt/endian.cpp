#include "endian.hpp"

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

}
