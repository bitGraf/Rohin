#pragma once

#include "NBT.hpp"

#include "Engine/Renderer/Mesh.hpp"

namespace nbtTest {

	void test1() {
		{
			using namespace nbt;
			
			tag_byte byteTest;
			tag_short shortTest;
			tag_int intTest;
			tag_long longTest;
			tag_float floatTest;
			tag_double doubleTest;
			tag_string stringTest;

			/*
			TAG_Compound("Level"): 11 entries
			{
			   TAG_Short("shortTest"): 32767
			   TAG_Long("longTest"): 9223372036854775807
			   TAG_Float("floatTest"): 0.49823147
			   TAG_String("stringTest"): HELLO WORLD THIS IS A TEST STRING ÅÄÖ!
			   TAG_Int("intTest"): 2147483647
			   TAG_Compound("nested compound test"): 2 entries
			   {
				  TAG_Compound("ham"): 2 entries
				  {
					 TAG_String("name"): Hampus
					 TAG_Float("value"): 0.75
				  }
				  TAG_Compound("egg"): 2 entries
				  {
					 TAG_String("name"): Eggbert
					 TAG_Float("value"): 0.5
				  }
			   }
			   TAG_List("listTest (long)"): 5 entries of type TAG_Long
			   {
				  TAG_Long: 11
				  TAG_Long: 12
				  TAG_Long: 13
				  TAG_Long: 14
				  TAG_Long: 15
			   }
			   TAG_Byte("byteTest"): 127
			   TAG_List("listTest (compound)"): 2 entries of type TAG_Compound
			   {
				  TAG_Compound: 2 entries
				  {
					 TAG_String("name"): Compound tag #0
					 TAG_Long("created-on"): 1264099775885
				  }
				  TAG_Compound: 2 entries
				  {
					 TAG_String("name"): Compound tag #1
					 TAG_Long("created-on"): 1264099775885
				  }
			   }
			   TAG_Byte_Array("byteArrayTest (the first 1000 values of (n*n*255+n*7)%100, starting with n=0 (0, 62, 34, 16, 8, ...))"): [1000 bytes]
			   TAG_Double("doubleTest"): 0.4931287132182315
			}
			*/

			std::ifstream file("bigtest.nbt", std::ios::binary);
			if (file) {
				std::cout << "Opened file!" << std::endl;

				auto pair = read_compound(file);

				auto name = pair.first;
				auto& comp = pair.second;

				file.close();

				// use the file now
				//nbt_byte byte_test = comp->at("byteTest").as<tag_byte>().get();
				//assert(comp->at("byteTest") == tag_byte(127));
				//assert(comp->at("byteTest").as<tag_byte>().get() == 127);

				/* output2.nbt should be identical to output.nbt */
				std::ofstream fileout("output2.nbt", std::ios::binary);
				if (fileout) {
					std::cout << "Writing nbt back to a file" << std::endl;
					//nbt::file_data pairout = { pair.first, std::make_unique<tag_compound>(pair.second) };
					write_compound(fileout, pair);
					fileout.close();
				}
			}

			bool done = true;
		}
	}

	void test2() {
		{
			// test if the nbt format can be easily used to store mesh data
			auto mesh = std::make_shared<Engine::Mesh>("run_tree/Data/Models/cube.mesh", true);

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
				nbt::write_compound(file, data);
				file.close();
			}

		}

		std::ifstream file("meshout.nbt", std::ios::binary);
		if (file) {
			auto data = nbt::read_compound(file);
			
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

}