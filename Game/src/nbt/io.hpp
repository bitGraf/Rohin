#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <memory>

#include "data.hpp"
#include "endian.hpp"

namespace nbt {
	class tag;
	class value;

	namespace io {
		const size_t max_array_len = 0x7FFFFFFF; // 2,147,483,647 entries
        const size_t max_name_len = 16;
        const size_t max_array_print = 8;

		tag_type read_type(std::istream& is);
		std::string read_string(std::istream& is, endian::endian e = endian::default_endian);
		std::unique_ptr<tag> read_payload(std::istream& is, tag_type type, endian::endian e = endian::default_endian);

		template<typename T>
		void read_num(std::istream& is, T& x, endian::endian e = endian::default_endian)
		{
			endian::read(is, x, e);
		}


		void write_type(std::ostream& os, tag_type type);
		void write_string(std::ostream& os, const std::string& text, endian::endian e);
		void write_payload(std::ostream& os, const tag& t, endian::endian e);
		void write_tag(std::ostream& os, const std::string& name, const nbt::value& tag, endian::endian e);

		template<typename T>
		void write_num(std::ostream& os, T& x, endian::endian e = endian::default_endian)
		{
			endian::write(os, x, e);
		}
	}
}