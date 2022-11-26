//#define RUN_TEST_CODE
//#define RUN_MATERIAL_CODE

#ifndef RUN_TEST_CODE
#include <Engine.hpp>
#include <Engine/EntryPoint.h>

// Scenes
#include "Scenes/MainMenu.hpp"
#include "Scenes/Level.hpp"

const int quickstartScene = 1;

class Game : public rh::Application {
public:
    Game() {
        switch (quickstartScene) {
            case 0: {
                // create MainMenu Scene
                MainMenuScene* m_MainMenuScene = new MainMenuScene();
                PushNewScene(m_MainMenuScene); // Application Class handles memory
            } break;
            case 1: {
                // create level scene
                Level* level = new Level("Level_1");
                PushNewScene(level);
            } break;
        }
    }

    ~Game() {
    }

private:
};

rh::Application* rh::CreateApplication() {
    return new Game();
}

#endif

#ifdef RUN_TEST_CODE
#ifndef RUN_MATERIAL_CODE

#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Engine/Core/GameMath.hpp"
#include <iostream>
#include <fstream>

// Use this moving forward
struct Vertex
{
    laml::Vec3 Position;
    laml::Vec3 Normal;
    laml::Vec3 Tangent;
    laml::Vec3 Binormal;
    laml::Vec2 Texcoord;
};

struct Triangle
{
    u32 V1, V2, V3;
};

class Submesh
{
public:
    //u32 BaseVertex;
    u32 BaseIndex;
    u32 MaterialIndex;
    u32 IndexCount;

    laml::Mat4 Transform;
};

//std::vector<Vertex> m_Vertices;
//m_Vertices.resize(numVerts);
//
//std::vector<Triangle> m_Tris;
//m_Tris.resize(numInds * 3);

// GLOBALS to mimic member vars
std::vector<Submesh> m_Submeshes;

bool LoadMeshData_FromMeshFile(const std::string& filename, std::vector<Vertex>& m_vertices, std::vector<Triangle>& m_tris) {
    const char KNOWN_VERSION_MAJOR = 0;
    const char KNOWN_VERSION_MINOR = 2;
    const char KNOWN_VERSION_PATCH = 0;

    // anonymous helper funcs
    auto checkTag = [](const char* tag, const char* comp, int len) {
        bool success = true;
        for (int n = 0; n < len; n++) {
            if (comp[n] != tag[n]) {
                success = false;
                break;
            }
        }
        if (!success) {
            switch (len) {
            case 4: {
                printf("ERROR::Expected '%s', got '%c%c%c%c'\n", comp, tag[0], tag[1], tag[2], tag[3]);
                break;
            } case 8: {
                printf("ERROR::Expected '%s', got '%c%c%c%c%c%c%c%c'\n", comp, tag[0], tag[1], tag[2], tag[3], tag[4], tag[5], tag[6], tag[7]);
                break;
            } default: {
                printf("ERROR::Expected '%s','\n", comp);
                break;
            }
            }
        }
        return success;
    };
    auto read_u32 = [](std::ifstream& file) -> u32 {
        u32 res;
        file.read(reinterpret_cast<char*>(&res), sizeof(u32));
        return res;
    };
    auto read_u16 = [](std::ifstream& file) -> u16 {
        u16 res;
        file.read(reinterpret_cast<char*>(&res), sizeof(u16));
        return res;
    };

    auto read_mat4 = [](std::ifstream& file, laml::Mat4* m) {
        file.read(reinterpret_cast<char*>(m), 16*sizeof(f32));
    };

    // Open file at the end
    std::ifstream file{ filename, std::ios::ate | std::ios::binary };

    if (!file.is_open()) {
        ENGINE_LOG_ERROR("Could not open file '{0}'!", filename);
        return false;
    }

    // read current offset (at end of file) giving filesize, then go back to the start
    size_t actual_fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0);

    // HEADER
    char MAGIC[4];
    file.read(MAGIC, 4);
    if (!checkTag(MAGIC, "MESH", 4)) return false;

    u32 FileSize = read_u32(file);
    if (static_cast<size_t>(FileSize) != actual_fileSize) printf("EROR::Expectied a fileSize of %ld, got %ld\n", actual_fileSize, FileSize);

    u32 Flag = read_u32(file);

    bool has_animations = Flag & 0x01;
    if (has_animations) {
        ENGINE_LOG_CRITICAL("Animated meshes not supported fully!!!!");
    }

    char INFO[4];
    file.read(INFO, 4);
    checkTag(INFO, "INFO", 4);

    u32 numVerts = read_u32(file);
    u32 numInds = read_u32(file);
    u16 numSubmeshes = read_u16(file);

    char vStr[4];
    file.read(vStr, 4);
    char vMajor = vStr[1];
    char vMinor = vStr[2];
    char vPatch = vStr[3];
    ENGINE_LOG_INFO("File version: v{0}.{1}.{2}", vMajor, vMinor, vPatch);
    ENGINE_LOG_INFO("Highest known version is: v{0}.{1}.{2}", KNOWN_VERSION_MAJOR, KNOWN_VERSION_MINOR, KNOWN_VERSION_PATCH);

    //if ( vMajor > KNOWN_VERSION_MAJOR || (vMajor <= KNOWN_VERSION_MAJOR && vMinor > KNOWN_VERSION_MINOR) ) {
    //	printf("File version too high! Highest known version is: v%i.%i.%i\n", KNOWN_VERSION_MAJOR, KNOWN_VERSION_MINOR, KNOWN_VERSION_PATCH);
    //}

    u16 len = read_u16(file);
    char* comment = (char*)malloc(len);
    file.read(comment, len);
    comment[len - 1] = 0; // just in case...
    ENGINE_LOG_INFO("  Embedded comment: '%s'", comment);
    free(comment);

    ENGINE_LOG_INFO("# of Vertices: {0}", numVerts);
    ENGINE_LOG_INFO("# of Indices:  {0}", numInds);
    ENGINE_LOG_INFO("# of Meshes:   {0}", numSubmeshes);

    m_vertices.resize(numVerts);
    m_tris.resize(numInds * 3);

    m_Submeshes.resize(numSubmeshes);

    // read each submesh
    for (int n_submesh = 0; n_submesh < numSubmeshes; n_submesh++) {
        char tag[8];
        file.read(tag, 8);
        checkTag(tag, "SUBMESH", 8);

        auto& sm = m_Submeshes[n_submesh];
        sm.BaseIndex = read_u32(file);
        sm.MaterialIndex = read_u32(file);
        sm.IndexCount = read_u32(file);
        read_mat4(file, &sm.Transform);
    }

    // DATA block
    char DATA[4];
    file.read(DATA, 4);
    checkTag(DATA, "DATA", 4);

    // read vertices
    char VERT[4];
    file.read(VERT, 4);
    checkTag(VERT, "VERT", 4);
    for (int n_vert = 0; n_vert < numVerts; n_vert++) {
        file.read(reinterpret_cast<char*>(&m_vertices[n_vert].Position), 3 * sizeof(f32));
        file.read(reinterpret_cast<char*>(&m_vertices[n_vert].Normal),   3 * sizeof(f32));
        file.read(reinterpret_cast<char*>(&m_vertices[n_vert].Tangent),  3 * sizeof(f32));
        file.read(reinterpret_cast<char*>(&m_vertices[n_vert].Binormal), 3 * sizeof(f32));
        file.read(reinterpret_cast<char*>(&m_vertices[n_vert].Texcoord), 2 * sizeof(f32));
    }

    // read indices
    char INDS[4];
    file.read(INDS, 4);
    checkTag(INDS, "IDX", 4);
    for (int n_tri = 0; n_tri < numInds / 3; n_tri++) {
        // Indices in groups of three
        m_tris[n_tri].V1 = read_u32(file);
        m_tris[n_tri].V2 = read_u32(file);
        m_tris[n_tri].V3 = read_u32(file);
    }

    // Closing tag
    char END[4];
    file.read(END, 4);

    if (END[0] != 'E' || END[1] != 'N' || END[2] != 'D' || END[3] != '\0') {
        ENGINE_LOG_ERROR("ERROR::Did not reach the END tag at the end!!\n");
    }

    // TODO: if any errrors happen, the file will not be closed properly
    file.close();

    return true;
}

int main(int argc, char** argv) {
    rh::Logger::Init();

    const std::string filename = "Data/Models/output.mesh";
    std::vector<Vertex> m_vertices;
    std::vector<Triangle> m_tris;

    bool success = LoadMeshData_FromMeshFile(filename, m_vertices, m_tris);
    if (!success)
        printf("Failed to lload data\n");

    system("pause");
    return 0;
}

#endif

#ifdef RUN_MATERIAL_CODE
#include <stdlib.h>
#include "Engine/Sound/SoundEngine.hpp"
#include "Engine/Sound/SoundDevice.hpp"
#include "Engine/Sound/SoundContext.hpp"
#include "Engine/Sound/SoundSource.hpp"
#include "Engine/Sound/SoundBuffer.hpp"
#include "Engine/Sound/SoundFileFormats.hpp"
#include "Engine/Sound/SoundStream.hpp"
using namespace rh;

#include "Engine/Resources/nbt/nbt.hpp"
using namespace nbt;

int main(int argc, char** argv) {

    //tag_compound mat_copper{
    //    { "name", "PBR Copper" },
    //    { "albedo_path",    "Data/Images/copper/albedo.png" },
    //    { "normal_path",    "Data/Images/copper/normal.png" },
    //    { "ambient_path",   "Data/Images/copper/ao.png" },
    //    { "metalness_path", "Data/Images/copper/metallic.png" },
    //    { "roughness_path", "Data/Images/copper/roughness.png" }
    //};
    //
    //tag_compound mat_concrete{
    //    { "name", "Waffle Concrete" },
    //    { "albedo_path",    "Data/Images/waffle/WaffleSlab2_albedo.png" },
    //    { "normal_path",    "Data/Images/waffle/WaffleSlab2_normal.png" },
    //    { "ambient_path",   "Data/Images/waffle/WaffleSlab2_ao.png" },
    //    { "roughness_path", "Data/Images/waffle/WaffleSlab2_roughness.png" }
    //};

    //tag_compound mat_damaged_helmet{
    //    { "name", "Damaged Helmet" },
    //    { "albedo_path",    "Data/Images/helmet/albedo.png" },
    //    { "normal_path",    "Data/Images/helmet/normal.png" },
    //    { "ambient_path",   "Data/Images/helmet/ambient.png" },
    //    { "metalness_path", "Data/Images/helmet/metallic.png" },
    //    { "roughness_path", "Data/Images/helmet/roughness.png" },
    //    { "emissive_path",  "Data/Images/helmet/emission.png" }
    //};

    //tag_compound mat_tape_recorder {
    //    { "name", "Tape Player" },
    //    { "albedo_path",    "Data/Images/tape/albedo.png" },
    //    //{ "normal_path",    "Data/Images/tape/normal.png" },
    //    { "ambient_path",   "Data/Images/tape/ambient.png" },
    //    { "metalness_path", "Data/Images/tape/metallic.png" },
    //    { "roughness_path", "Data/Images/tape/roughness.png" },
    //    { "emissive_path",  "Data/Images/tape/emission.png" }
    //};

    tag_compound mat_guard{
        { "name", "Guard Man" },
        { "albedo_path",    "Data/Images/guard.png" }
    };

    tag_compound material_catalog{
        //{ "mat_copper", tag_compound(mat_copper) },
        //{ "mat_concrete", tag_compound(mat_concrete) },
        //{ "mat_damaged_helmet", tag_compound(mat_damaged_helmet) },
        //{ "mat_tape_recorder", tag_compound(mat_tape_recorder) },
        { "mat_guard", tag_compound(mat_guard) }
    };

    auto result = nbt::write_to_file("Data/Materials/materials.nbt", nbt::file_data({ "material_catalog", std::make_unique<tag_compound>(material_catalog) }), 0, 1);
    if (!result)
        __debugbreak();

    system("pause");

#if 0
    auto devs = SoundDevice::GetDevices();
    for (auto d : devs)
        printf("%s\n", d.c_str());
    Ref<SoundDevice> device = SoundDevice::Create();
    device->Open();

    Ref<SoundContext> context = SoundContext::Create(device);
    context->MakeCurrent();

    Ref<SoundStream> stream = SoundStream::Create("Data/Sounds/ahhh.ogg");
    stream->PlayStream();

    while (true) {
        stream->UpdateStream();
    }

    // cleanup
    context->MakeCurrent();
    context->Destroy();
    device->Close();

    return 0;
#endif
}
#endif
#endif