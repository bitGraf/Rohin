//#define RUN_TEST_CODE
#ifndef RUN_TEST_CODE
#include <Engine.hpp>
#include <Engine/EntryPoint.h>

#include "GameLayer.hpp"
#include "MenuLayer.hpp"

class Game : public Engine::Application {
public:
    Game() {
        m_GameLayer = new GameLayer();
        PushLayer(m_GameLayer);

        //m_MainMenuLayer = new MenuLayer();
        //m_MainMenuLayer->createMenu();
        //PushLayer(m_MainMenuLayer);
    }

    ~Game() {
    }

private:
    MenuLayer* m_MainMenuLayer;
    GameLayer* m_GameLayer;
};

Engine::Application* Engine::CreateApplication() {
    return new Game();
}
#endif

#ifdef RUN_TEST_CODE
#include "nbt\nbt.hpp"

#include "Engine\Core\GameMath.hpp"
#include "nbt\test.hpp"

int main(int argc, char** argv) {
    // try to generate a level file in .nbt format

    using namespace nbt;
    /*
    tag_compound comp{
        {"name", "Level NBT Test"},
        {"meshes", tag_list::of<tag_compound>({
            {{"mesh_name", "cube_mesh"}, {"mesh_path", "run_tree/Data/Models/cube.mesh" }},
            {{"mesh_name", "rect_mesh"}, {"mesh_path", "run_tree/Data/Models/cube.mesh" }}
            })},
        {"entities", tag_list::of<tag_compound>({
            {{"name", "Platform"}, {"components", tag_list::of<tag_compound>({
                {{"type", "Transform"}, {"mesh_name", "cube_mesh"}},
                {{"type", "MeshRenderer"}, {"mesh_name", "cube_mesh"}}
                })}}
            })}
    };
    */
    nbtTest::test5();
}

#endif

#if 0
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <assert.h>
#include "Engine/Core/GameMath.hpp"
#include "Engine/Core/DataFile.hpp"
using namespace math;

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec3 Tangent;
    vec3 Binormal;
    vec2 Texcoord;
};

int main(int argc, char** argv) {
    std::cout << "Running..." << std::endl;
    DataFile file;
    DataFile fin;

    /* Write to the file */
    {
        /* Create mesh data */
        std::vector<Vertex> vertices;
        vertices.resize(8);
        std::vector<u32> indices;
        {
            indices = {
                0, 1, 5,
                0, 5, 4,
                1, 2, 6,
                1, 6, 5,
                2, 3, 7,
                2, 7, 6,
                3, 0, 4,
                3, 4, 7,
                4, 5, 6,
                4, 6, 7,
                1, 0, 3,
                1, 3, 2
            };

            scalar s = 0.5f;
            vertices[0].Position = { -s, -s,  s };
            vertices[1].Position = { s, -s,  s };
            vertices[2].Position = { s, -s, -s };
            vertices[3].Position = { -s, -s, -s };
            vertices[4].Position = { -s,  s,  s };
            vertices[5].Position = { s,  s,  s };
            vertices[6].Position = { s,  s, -s };
            vertices[7].Position = { -s,  s, -s };

            for (auto& v : vertices) {
                v.Normal = v.Position.get_unit();
                v.Tangent = vec3(0, 1, 0);
                v.Binormal = v.Normal.cross(v.Tangent);
                v.Tangent = v.Binormal.cross(v.Normal);
            }

            vertices[0].Texcoord = { 0, 0 };
            vertices[1].Texcoord = { 1, 0 };
            vertices[5].Texcoord = { 1, 1 };
            vertices[4].Texcoord = { 0, 1 };

            vertices[2].Texcoord = { 0, 0 };
            vertices[3].Texcoord = { 1, 0 };
            vertices[7].Texcoord = { 1, 1 };
            vertices[6].Texcoord = { 0, 1 };
        }

        // Materials
        DataFile::Block matBlock("Materials");
        {
            // base material data
            matBlock.push<vec3>(vec3(1,1,1)); // u_AlbedoColor
            matBlock.push<float>(0.0f); // u_Metalness
            matBlock.push<float>(0.75f); // u_Roughness

            u8 numTextures = 4;
            matBlock.push_byte(numTextures);

            // AlbedoTexture
            std::string albedoPath = "run_tree/Data/Images/frog.png";
            std::string normalPath = "run_tree/Data/Images/frog.png";
            std::string metalPath = "run_tree/Data/Images/frog.png";
            std::string roughnessPath = "run_tree/Data/Images/frog.png";
            
            matBlock.push_string("u_AlbedoTexture");
            matBlock.push_string(albedoPath);
            matBlock.push_string("u_NormalTexture");
            matBlock.push_string(normalPath);
            matBlock.push_string("u_MetalnessTexture");
            matBlock.push_string(metalPath);
            matBlock.push_string("u_RoughnessTexture");
            matBlock.push_string(roughnessPath);

            u8 numMaterials = 2;
            matBlock.push_byte(numMaterials);

            /// mat1
            std::string mat1Name = "mat1";
            matBlock.push_string(mat1Name);

            matBlock.push<vec3>(vec3(1, .5, .5)); // u_AlbedoColor
            matBlock.push<float>(0.0f); // u_Metalness
            matBlock.push<float>(0.75f); // u_Roughness

            matBlock.push_byte(0); // numTextures

            /// mat2
            std::string mat2Name = "mat2";
            matBlock.push_string(mat2Name);

            matBlock.push<vec3>(vec3(.5, 1, .5)); // u_AlbedoColor
            matBlock.push<float>(0.0f); // u_Metalness
            matBlock.push<float>(0.75f); // u_Roughness

            matBlock.push_byte(0); // numTextures
        }

        // Mesh
        DataFile::Block meshBlock("Mesh");
        {
            std::string meshName = "Cube";
            meshBlock.push_string(meshName);

            u8 numSubmeshes = 2;
            u16 numVerts = vertices.size();
            u16 numInds = indices.size();
            meshBlock.push_byte(numSubmeshes);
            meshBlock.push_short(numVerts);
            meshBlock.push_short(numInds);
            meshBlock.push_data(reinterpret_cast<u8*>(&vertices[0]), vertices.size() * sizeof(vertices[0]));
            meshBlock.push_data(reinterpret_cast<u8*>(&indices[0]), indices.size() * sizeof(indices[0]));

            u8 meshFlag = 0;
            meshBlock.push_byte(meshFlag);

            /// submesh 1
            std::string sm1Name = "subcube1";
            u8 sm1Index = 0;
            mat4 sm1Transform({ 1,0,0,0 }, { 0, 1,0,0 }, { 0,0,1,0 }, { 0,0,0,1 });
            u16 sm1startIndex = 0;
            u16 sm1indexCount = 18;
            meshBlock.push_string(sm1Name);
            meshBlock.push_byte(sm1Index);
            meshBlock.push_data(reinterpret_cast<u8*>(&sm1Transform._11), sizeof(sm1Transform));
            meshBlock.push_short(sm1startIndex);
            meshBlock.push_short(sm1indexCount);

            /// submesh 2
            std::string sm2Name = "subcube2";
            u16 sm2Index = 1;
            mat4 sm2Transform({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { 0,0,0,1 });
            u16 sm2startIndex = 18;
            u16 sm2indexCount = 18;
            meshBlock.push_string(sm2Name);
            meshBlock.push_byte(sm2Index);
            meshBlock.push_data(reinterpret_cast<u8*>(&sm2Transform._11), sizeof(sm2Transform));
            meshBlock.push_short(sm2startIndex);
            meshBlock.push_short(sm2indexCount);
        }

        // Skeleton
        DataFile::Block skelBlock("Skeleton");
        {
            std::string skelName = "CubeSkeleton";
            skelBlock.push_string(skelName);
            u8 numBones = 77;
            mat4 invTransform({ 1,2,3,4 }, { 5, 6, 7, 8 }, { 9, 10, 11, 12 }, { 13, 14, 15, 16 });
            skelBlock.push_byte(numBones);
            skelBlock.push_data(reinterpret_cast<u8*>(&invTransform._11), sizeof(invTransform));
        }

        // Prepare for writing
        file.AddBlock(matBlock);
        file.AddBlock(meshBlock);
        file.AddBlock(skelBlock);
        file.ResolveOffsets();
        file.WriteToFile("cube.mesh");
    }

    /* Read from the file and extract data */
    {
        fin.ReadFromFile("cube.mesh");

        {
            auto& block = fin.GetBlock("Skeleton");
            auto skelName = block.read_string();
            auto numBones = block.read_byte();
            mat4 invTransform = block.read<mat4>();
        }

        /*
        {
            auto& block = fin.GetBlock("Materials");
            auto baseColor = block.read<vec4>();
            auto numTextures = block.read_byte();
            for (int n = 0; n < numTextures; n++) {
                auto texName = block.read_string();
                auto texPath = block.read_string();
            }
            auto numMats = block.read_byte();
            for (int n = 0; n < numMats; n++) {
                auto matName = block.read_string();
                vec4 u_color = block.read<vec4>();
                auto numTextures = block.read_byte();
                for (int n = 0; n < numTextures; n++) {
                    auto texName = block.read_string();
                    auto texPath = block.read_string();
                }
            }
        }
        */

        {
            auto& block = fin.GetBlock("Mesh");
            auto meshName = block.read_string();
            auto numSubmeshes = block.read_byte();
            auto numVerts = block.read_short();
            auto numInds = block.read_short();

            Vertex* __new_vertices_ = new Vertex[numVerts];
            block.read_data(reinterpret_cast<u8*>(__new_vertices_), numVerts * sizeof(Vertex));
            std::vector<Vertex> verts;
            verts.reserve(numVerts);
            verts.assign(__new_vertices_, __new_vertices_ + numVerts);
            delete[] __new_vertices_;

            u32* __new_indices_ = new u32[numInds];
            block.read_data(reinterpret_cast<u8*>(__new_indices_), numInds * sizeof(u32));
            std::vector<u32> inds;
            inds.reserve(numInds);
            inds.assign(__new_indices_, __new_indices_ + numInds);
            delete[] __new_indices_;

            auto meshFlag = block.read_byte();

            // load submeshes
            for (int n = 0; n < numSubmeshes; n++) {
                auto smName = block.read_string();
                auto smMatIdx = block.read_byte();
                auto smTransf = block.read<mat4>();
                auto smStartIdx = block.read_short();
                auto smIdxCount = block.read_short();
            }
        }
    }

    return 0;
}

#endif