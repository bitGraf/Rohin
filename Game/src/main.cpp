//#define RUN_TEST_CODE
//#define RUN_MATERIAL_CODE

#ifndef RUN_TEST_CODE
#include <Engine.hpp>
#include <Engine/EntryPoint.h>

// Scenes
#include "Scenes/MainMenu.hpp"
#include "Scenes/Level.hpp"

const int quickstartScene = 3;

class Game : public Engine::Application {
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
            case 2: {
                // create level scene
                Level* level = new Level("Level_2");
                PushNewScene(level);
            } break;
            case 3: {
                // create level scene
                Level* level = new Level("Level_3");
                PushNewScene(level);
            } break;
        }
    }

    ~Game() {
    }

private:
};

Engine::Application* Engine::CreateApplication() {
    return new Game();
}

#endif

#ifdef RUN_TEST_CODE
#ifndef RUN_MATERIAL_CODE

#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_map>

struct Mesh {
    int x;
    float y;

    void load(const std::string& f) {}
};

struct mCatalog {
    std::unordered_map<std::string, Mesh*> Data;

    void Register(const std::string& key, const std::string& filename) {
        // check if this key is already registered
        if (Data.find(key) == Data.end()) {
            // not registered yet.
            Mesh* newMesh = new Mesh();
            newMesh->load(filename);
            Data.emplace(key, newMesh);
        }
    }

    void Create() {
        // memory pool??
        Data.clear();
    }

    void Destroy() {
        for (const auto& it : Data) {
            delete it.second;
        }
        Data.clear();
    }

    Mesh* Get(const std::string& key) {
        // check if this key exists
        if (Data.find(key) == Data.end()) {
            return nullptr;
        }

        return Data[key];
    }
};

int main(int argc, char** argv) {

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
using namespace Engine;

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