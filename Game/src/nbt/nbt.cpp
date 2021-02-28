#include "nbt.hpp"

#include <assert.h>

namespace nbt {
	std::pair<std::string, std::unique_ptr<tag_compound>> read_compound(std::istream& is, endian::endian e) {
		nbt_byte tag;
		io::read_num(is, tag, e);
		assert(tag == (nbt_byte)tag_type::Compound);
		std::string key = io::read_string(is, e);
		auto comp = std::make_unique<tag_compound>();
		comp->read_payload(is, e);
		return { std::move(key), std::move(comp) };
	}

	void write_compound(std::ostream& os, file_data& data, endian::endian e) {
		nbt_byte tag = (nbt_byte)tag_type::Compound;
		io::write_num(os, tag, e);
		std::string name = data.first;
		io::write_string(os, name, e);
		auto& comp = data.second;
		comp->write_payload(os, e);
	}
}