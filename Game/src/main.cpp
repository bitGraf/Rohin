#define RUN_TEST_CODE
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

#include "Engine/Core/Base.hpp"
#include "Engine.hpp"

int main(int argc, char** argv) {
    rh::Logger::Init();
    LOG_INFO("Logging enabled!");

    //template<typename T>
    //bool decompose(const Matrix<T, 4, 4> & transform,
    //    Quaternion<T> & rot_quat, Vector<T, 3> & pos_vec, Vector<T, 3> & scale_vec) {
    rh::laml::Mat4 transform(1.0f);
    rh::laml::transform::create_transform(transform, 
        rh::laml::normalize(rh::laml::Quat(1.0f, 2.0f, -0.5f, -1.5f)), 
        rh::laml::Vec3(2.5f, 2.5f, 4.5f), 
        rh::laml::Vec3(1.0f, 2.0f, 3.0f));

    rh::laml::Quat rot_quat;
    rh::laml::Vec3 pos_vec;
    rh::laml::Vec3 scale_vec;
    rh::laml::transform::decompose(transform, rot_quat, pos_vec, scale_vec);

    //system("pause");
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