#pragma once

#include "Engine\Resources\nbt\NBT.hpp"

#ifdef ROHIN_GAME
    #include "Engine/Renderer/Mesh.hpp"
#endif

namespace nbtTest {

	void test1() {
		{
			using namespace nbt;

			std::ifstream file("bigtest.nbt", std::ios::binary);
			if (file) {
				std::cout << "Opened file!" << std::endl;

				auto pair = read_compound_raw(file);

				auto name = pair.first;
				auto& comp = pair.second;

				file.close();

				/* output2.nbt should be identical to output.nbt */
				std::ofstream fileout("output2.nbt", std::ios::binary);
				if (fileout) {
					std::cout << "Writing nbt back to a file" << std::endl;
					//nbt::file_data pairout = { pair.first, std::make_unique<tag_compound>(pair.second) };
					write_compound_raw(fileout, pair);
					fileout.close();
				}
			}

			bool done = true;
		}
	}

#ifdef ROHIN_GAME
	void test2() {
		{
			// test if the nbt format can be easily used to store mesh data
			auto mesh = std::make_shared<Engine::Mesh>("Data/Models/cube.mesh", true);

			auto verts = mesh->GetVertices();
			auto inds = mesh->GetIndices();

			std::string name = "mesh_cube";
			//std::string name = mesh->GetName();
			nbt::tag_compound mesh_data;

			/*
			typedef f32 scalar;

			struct vec3 {
				scalar x, y, z;
			};

			struct Vertex {
				math::vec3 Position;
				math::vec3 Normal;
				math::vec3 Tangent;
				math::vec3 Binormal;
				math::vec2 Texcoord;
			};
			*/

			mesh_data["numSubmeshes"] = nbt::tag_byte(mesh->GetSubmeshes().size());
			mesh_data["numVerts"] = nbt::tag_short(verts.size());
			mesh_data["numInds"] = nbt::tag_short(inds.size() * 3);

			std::vector<nbt::nbt_byte> vert_byte_vec;
			vert_byte_vec.reserve(verts.size() * sizeof(Engine::Vertex));
			vert_byte_vec.assign(reinterpret_cast<nbt::nbt_byte*>(verts.data()), reinterpret_cast<nbt::nbt_byte*>(verts.data()) + (verts.size() * sizeof(Engine::Vertex)));
			mesh_data["vertices"] = nbt::tag_byte_array(std::move(vert_byte_vec));

			std::vector<nbt::nbt_byte> ind_byte_vec;
			ind_byte_vec.reserve(inds.size() * sizeof(Engine::Index));
			ind_byte_vec.assign(reinterpret_cast<nbt::nbt_byte*>(inds.data()), reinterpret_cast<nbt::nbt_byte*>(inds.data()) + (inds.size() * sizeof(Engine::Index)));
			mesh_data["indices"] = nbt::tag_byte_array(std::move(ind_byte_vec));

			mesh_data["meshFlag"] = nbt::nbt_byte(0); // placeholder for now

			std::ofstream file("meshout.nbt", std::ios::binary);

			if (file) {
				nbt::file_data data{ name, std::make_unique<nbt::tag_compound>(mesh_data) };
				nbt::write_compound_raw(file, data);
				file.close();
			}

		}

		std::ifstream file("meshout.nbt", std::ios::binary);
		if (file) {
			auto data = nbt::read_compound_raw(file);
			
			std::cout << "Data name: " << data.first << std::endl;
			for (auto& tag : *data.second) {
				std::cout << " tag[" << tag.second.get_type() << "] ";
				if (tag.second.get_type() != nbt::tag_type::End) {
					std::cout << tag.first;
				}
				std::cout << std::endl;
			}

			system("pause");
			file.close();
		}
	}
#endif

    // read bigtest.nbt as a raw compound and std::cout it to see its tree
    void test3() {
        std::ifstream file_in("bigtest.nbt", std::ios::binary);

        if (file_in) {
            auto data = nbt::read_compound_raw(file_in);

            auto comp_name = data.first;
            auto& comp_data = data.second;

            std::cout << "tag[" << comp_data->get_type() << "] " << comp_name << comp_data->as<nbt::tag_compound>() << std::endl;

            file_in.close();
        }

        system("pause");
    }

    // try writing to a full .nbt file with header
    void test4() {
        std::ifstream file_in("bigtest.nbt", std::ios::binary);

        if (!file_in) {
            std::cout << "Failed to load bigtest.nbt!" << std::endl;
            system("pause");
            return;
        }

        auto data = nbt::read_compound_raw(file_in);
        file_in.close();

        bool write_success = nbt::write_to_file(
            "output.nbt_file",
            data,
            0, 1, endian::little);


        // read from the same file
        nbt::file_data data_in;
        nbt::nbt_byte version_major, version_minor;
        endian::endian endianness;
        bool read_success = nbt::read_from_file(
            "output.nbt_file",
            data_in,
            version_major, version_minor, endianness);

        if (!write_success)
            std::cout << "failed to write to file!" << std::endl;
        if (!read_success)
            std::cout << "failed to read from file!" << std::endl;

        system("pause");
    }

    // implement math structs as primitives
    void test5() {
        using namespace nbt;
        tag_compound comp{
            { "name", "Level NBT Test" },
            { "position", math::vec3(1,2,3)} ,
            {"matrix", math::mat4(1,2,3.4,4)},
            {"verts", tag_list::of<tag_vec3>({
                math::vec3(1,2,3),
                math::vec3(2,3,4),
                math::vec3(3,4,5)
                })}
        };

        std::cout << comp["matrix"].as<tag_mat4>() << std::endl;

        auto v = comp["position"].as<tag_vec3>().get();
        std::cout << v << std::endl;
        comp["position"].set(tag_vec3(math::vec3(2,4,5)));
        v = comp["position"].as<tag_vec3>().get();
        std::cout << v << std::endl << std::endl;

        std::cout << comp << std::endl; // print whole compound

        auto list = comp["verts"].as<tag_list>();
        std::cout << comp["verts"].as<tag_list>() << std::endl; // print tag_list
        
        for (auto& a : list) {
            std::cout << a.as<tag_vec3>() << std::endl; // print each element in the list
        }

        system("pause");
    }
}