#pragma once

#include "data.hpp"
#include "tag.hpp"

namespace nbt {
	typedef std::pair<std::string, std::unique_ptr<tag_compound>> file_data;

	file_data read_compound(std::istream& is, endian::endian e = endian::default_endian);
	void write_compound(std::ostream& os, file_data& data, endian::endian e = endian::default_endian);
	
}