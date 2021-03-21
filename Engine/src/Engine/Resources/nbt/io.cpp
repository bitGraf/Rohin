#include <enpch.hpp>
#include "Engine/Resources/nbt/io.hpp"
#include "Engine/Resources/nbt/tag.hpp"

namespace nbt {
	namespace io {
		tag_type read_type(std::istream& is) {
			nbt_byte t;
			read_num(is, t);
			return (tag_type)t;
		}

		std::string read_string(std::istream& is, endian::endian e) {
			uint16_t len;
			read_num(is, len, e);
			if (!is)
				__debugbreak();

			std::string ret(len, '\0');
			is.read(&ret[0], len);
			if (!is)
				__debugbreak();
			return ret;
		}

		std::unique_ptr<tag> read_payload(std::istream& is, tag_type type, endian::endian e) {
			std::unique_ptr<tag> t = tag::create(type);
			t->read_payload(is, e);
			return t;
		}

		void write_type(std::ostream& os, tag_type type) {
			nbt_byte t = (nbt_byte)type;
			write_num(os, t);
		}

		void write_payload(std::ostream& os, const tag& t, endian::endian e) {
			t.write_payload(os, e); 
		}

		void write_tag(std::ostream& os, const std::string& name, const value& tag, endian::endian e) {
			write_type(os, tag.get_type());
			write_string(os, name, e);
			write_payload(os, tag, e);
		}

		void write_string(std::ostream& os, const std::string& text, endian::endian e) {
			nbt_short _len = text.length();
			io::write_num(os, _len, e);
			os.write(&text[0], text.length());
		}
	}
}