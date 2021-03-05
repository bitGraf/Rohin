#pragma once

#include <fstream>

// forward declare math primitives
namespace math { 
    class vec2; 
    class vec3;
    class vec4;

    class mat2;
    class mat3;
    class mat4;
}

namespace endian {
	enum endian { little, big };
	constexpr endian default_endian = endian::big;

	///Writes number to stream in little endian
	void write_little(std::ostream& os, uint8_t x);
	void write_little(std::ostream& os, uint16_t x);
	void write_little(std::ostream& os, uint32_t x);
	void write_little(std::ostream& os, uint64_t x);
	void write_little(std::ostream& os, int8_t x);
	void write_little(std::ostream& os, int16_t x);
	void write_little(std::ostream& os, int32_t x);
	void write_little(std::ostream& os, int64_t x);
	void write_little(std::ostream& os, float x);
	void write_little(std::ostream& os, double x);
    void write_little(std::ostream& os, math::vec2 x);
    void write_little(std::ostream& os, math::vec3 x);
    void write_little(std::ostream& os, math::vec4 x);
    void write_little(std::ostream& os, math::mat2 x);
    void write_little(std::ostream& os, math::mat3 x);
    void write_little(std::ostream& os, math::mat4 x);

	///Writes number to stream in big endian
	void write_big(std::ostream& os, uint8_t x);
	void write_big(std::ostream& os, uint16_t x);
	void write_big(std::ostream& os, uint32_t x);
	void write_big(std::ostream& os, uint64_t x);
	void write_big(std::ostream& os, int8_t x);
	void write_big(std::ostream& os, int16_t x);
	void write_big(std::ostream& os, int32_t x);
	void write_big(std::ostream& os, int64_t x);
	void write_big(std::ostream& os, float x);
	void write_big(std::ostream& os, double x);
    void write_big(std::ostream& os, math::vec2 x);
    void write_big(std::ostream& os, math::vec3 x);
    void write_big(std::ostream& os, math::vec4 x);
    void write_big(std::ostream& os, math::mat2 x);
    void write_big(std::ostream& os, math::mat3 x);
    void write_big(std::ostream& os, math::mat4 x);

	///Writes number to stream in specified endian
	template<class T>
	void write(std::ostream& os, T x, endian e)
	{
		if (e == little)
			write_little(os, x);
		else
			write_big(os, x);
	}

	///Reads number from stream in little endian
	void read_little(std::istream& is, uint8_t& x);
	void read_little(std::istream& is, uint16_t& x);
	void read_little(std::istream& is, uint32_t& x);
	void read_little(std::istream& is, uint64_t& x);
	void read_little(std::istream& is, int8_t& x);
	void read_little(std::istream& is, int16_t& x);
	void read_little(std::istream& is, int32_t& x);
	void read_little(std::istream& is, int64_t& x);
	void read_little(std::istream& is, float& x);
	void read_little(std::istream& is, double& x);
    void read_little(std::istream& is, math::vec2& x);
    void read_little(std::istream& is, math::vec3& x);
    void read_little(std::istream& is, math::vec4& x);
    void read_little(std::istream& is, math::mat2& x);
    void read_little(std::istream& is, math::mat3& x);
    void read_little(std::istream& is, math::mat4& x);

	///Reads number from stream in big endian
	void read_big(std::istream& is, uint8_t& x);
	void read_big(std::istream& is, uint16_t& x);
	void read_big(std::istream& is, uint32_t& x);
	void read_big(std::istream& is, uint64_t& x);
	void read_big(std::istream& is, int8_t& x);
	void read_big(std::istream& is, int16_t& x);
	void read_big(std::istream& is, int32_t& x);
	void read_big(std::istream& is, int64_t& x);
	void read_big(std::istream& is, float& x);
	void read_big(std::istream& is, double& x);
    void read_big(std::istream& is, math::vec2& x);
    void read_big(std::istream& is, math::vec3& x);
    void read_big(std::istream& is, math::vec4& x);
    void read_big(std::istream& is, math::mat2& x);
    void read_big(std::istream& is, math::mat3& x);
    void read_big(std::istream& is, math::mat4& x);

	///Reads number from stream in specified endian
	template<class T>
	void read(std::istream& is, T& x, endian e)
	{
		if (e == little)
			read_little(is, x);
		else
			read_big(is, x);
	}
}