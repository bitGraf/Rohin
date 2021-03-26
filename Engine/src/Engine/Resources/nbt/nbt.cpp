#include <enpch.hpp>
#include "Engine/Resources/nbt/nbt.hpp"

#include <assert.h>

namespace nbt {
	std::pair<std::string, std::unique_ptr<tag_compound>> read_compound_raw(std::istream& is, endian::endian e) {
		nbt_byte tag;
		io::read_num(is, tag, e);
		assert(tag == (nbt_byte)tag_type::Compound);
		std::string key = io::read_string(is, e);
		auto comp = std::make_unique<tag_compound>();
		comp->read_payload(is, e);
		return { std::move(key), std::move(comp) };
	}

	void write_compound_raw(std::ostream& os, file_data& data, endian::endian e) {
		nbt_byte tag = (nbt_byte)tag_type::Compound;
		io::write_num(os, tag, e);
		std::string name = data.first;
		io::write_string(os, name, e);
		auto& comp = data.second;
		comp->write_payload(os, e);
	}


    // version specific write functions
    bool write_to_file_dispatch(
        std::ofstream& file_out,
        nbt_byte version_major, nbt_byte version_minor,
        endian::endian endianness,
        file_data& data) {

        // write header
        /* 32 byte header
        [signature(4bytes)]
        [version_major(1byte)]
        [version_minor(1byte)]
        [endianness(1byte)]
        [padding(25bytes)]
        */
        nbt_byte header[32];
        memset(header, 0, 32);

        // signature
        char sig[] = "1234";
        memcpy(header, sig, 4);

        // version
        header[4] = version_major;
        header[5] = version_minor;

        // endianness
        header[6] = endianness == endian::big ? 1 : 0;

        // padding
        char pad[] = "this data isn't read...";
        memcpy(header + 7, pad, sizeof(pad));

        file_out.write(reinterpret_cast<char*>(header), 32);

        // write data normally
        write_compound_raw(file_out, data, endianness);

        switch (version_major) {
        case 0:
            switch (version_minor) {
            case 1:
                // nothing yet

                return true;
            default:
                std::cout << "Writing version " << version_major << "." << version_minor << " not supported yet" << std::endl;
                return false;
            }
        default:
            std::cout << "Writing version " << version_major << ".X not supported yet" << std::endl;
            return false;
        }
    }

    /* nbt write_to_file */
    bool write_to_file(
        const std::string& filename,
        file_data& data,
        nbt_byte version_major, nbt_byte version_minor,
        endian::endian endianness) {

        std::ofstream file_out(filename, std::ios::binary);
        if (file_out) {
            // write compound data
            bool write_success = write_to_file_dispatch(file_out, version_major, version_minor, endianness, data);

            // close file
            file_out.close();

            return true;
        }

        return false;
    }

    // version specific read functions
    bool read_from_file_dispatch(
        std::ifstream& file_in, 
        nbt_byte version_major, nbt_byte version_minor, 
        endian::endian endianness, 
        file_data& data, char* pad) {

        switch (version_major) {
        case 0:
            switch (version_minor) {
            case 1:
                // check that everything is as it should be

                // read data normally
                data = read_compound_raw(file_in, endianness);

                return true;
            default:
                std::cout << "Reading version " << version_major << "." << version_minor << " not supported yet" << std::endl;
                return false;
            }
        default:
            std::cout << "Reading version " << version_major << ".X not supported yet" << std::endl;
            return false;
        }
    }

    // read from file
    bool read_from_file(
        const std::string& filename,
        file_data& data,
        nbt_byte& major, nbt_byte& minor,
        endian::endian& endianness) {

        std::ifstream file_in(filename, std::ios::binary);
        if (file_in) {
            // read header
            /* 32 byte header
            [signature(4bytes)]
            [version_major(1byte)]
            [version_minor(1byte)]
            [endianness(1byte)]
            [padding(25bytes)]
            */
            nbt_byte header[32];
            file_in.read(reinterpret_cast<char*>(header), 32);

            // signature
            char sig[4];
            memcpy(sig, header, 4);
            assert(strncmp(sig, "1234", 4) == 0);

            // version
            major = header[4];
            minor = header[5];

            // endianness
            endianness = (header[6] == 1? endian::big : endian::little);

            // padding
            char pad[25];
            memcpy(pad, header + 7, sizeof(pad));

            // read compound data
            bool read_success = read_from_file_dispatch(file_in, major, minor, endianness, data, pad);

            // close file
            file_in.close();

            return read_success;
        }

        return false;

    }

    // Safe/Gauranteed accessors
    std::string SafeGetString(const tag_compound& comp, const std::string& key, const std::string& default) {
        if (comp.has_key(key)) {
            ENGINE_LOG_ASSERT(comp.at(key).get_type() == nbt::tag_type::String, "Value at key must be a string");
            return comp.at(key).as<tag_string>().get();
        }
        else {
            return default;
        }
    }

    math::vec3 SafeGetVec3(const tag_compound& comp, const std::string& key, const math::vec3& default) {
        if (comp.has_key(key)) {
            ENGINE_LOG_ASSERT(comp.at(key).get_type() == nbt::tag_type::Vector3, "Value at key must be a vec3");
            return comp.at(key).as<tag_vec3>().get();
        }
        else {
            return default;
        }
    }

    float SafeGetFloat(const tag_compound& comp, const std::string& key, float default) {
        if (comp.has_key(key)) {
            ENGINE_LOG_ASSERT(comp.at(key).get_type() == nbt::tag_type::Float, "Value at key must be a float");
            return comp.at(key).as<tag_float>().get();
        }
        else {
            return default;
        }
    }
}