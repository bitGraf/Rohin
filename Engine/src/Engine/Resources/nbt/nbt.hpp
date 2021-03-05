#pragma once

#include "Engine\Resources\nbt\data.hpp"
#include "Engine\Resources\nbt\tag.hpp"

namespace nbt {
	typedef std::pair<std::string, std::unique_ptr<tag_compound>> file_data;

	file_data read_compound_raw(std::istream& is, endian::endian e = endian::default_endian);
	void write_compound_raw(std::ostream& os, file_data& data, endian::endian e = endian::default_endian);
	
    // write to file
    bool write_to_file(
        const std::string& filename, 
        file_data& data,
        nbt_byte major, nbt_byte minor,
        endian::endian e = endian::default_endian);

    // read from file
    bool read_from_file(
        const std::string& filename,
        file_data& data,
        nbt_byte& major, nbt_byte& minor,
        endian::endian& endianness);
}