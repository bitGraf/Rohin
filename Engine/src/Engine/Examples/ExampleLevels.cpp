#include <enpch.hpp>

#include "ExampleLevels.hpp"
#include "Engine/Scene/Scene3D.hpp"

#include "Engine/GameObject/Components.hpp"
#include "Engine/Resources/MeshCatalog.hpp"
#include "Engine/Resources/MaterialCatalog.hpp"
#include "Engine/Core/Application.hpp"
#include "Engine/Sound/SoundEngine.hpp"

namespace Engine {

    void load_level_1(Scene3D* scene);
    void load_level_2(Scene3D* scene);
    void load_level_3(Scene3D* scene);

    bool LoadExampleLevel(const std::string& levelName, Scene3D* scene) {
        if (levelName.compare("Level_1") == 0) {
            load_level_1(scene);
            return true;
        } else if (levelName.compare("Level_2") == 0) {
            load_level_2(scene);
            return true;
        } else if (levelName.compare("Level_3") == 0) {
            load_level_3(scene);
            return true;
        }

        // could not find hard-coded level
        return false;
    }


    void load_level_1(Scene3D* scene) {
        Engine::MeshCatalog::Register("mesh_guard", "Data/Models/guard.nbt", true);
        Engine::MeshCatalog::Register("mesh_plane", "Data/Models/plane.nbt", true);
        auto& cWorld = scene->GetCollisionWorld();

        { // Player
            auto player = scene->CreateGameObject("Player");
            auto mesh = MeshCatalog::Get("mesh_guard");
            player.AddComponent<MeshRendererComponent>(mesh);
            //player.AddComponent<NativeScriptComponent>().Bind<PlayerController>(player);
            BindGameScript("script_player_controller", scene, player);

            mesh->GetSubmeshes()[0].Transform = math::createYawPitchRollMatrix(90, 0, 0);

            auto& trans = player.GetComponent<TransformComponent>().Transform;
            trans = math::mat4();
            trans.translate(math::vec3(0, 1, 0));

            UID_t hull = cWorld.CreateNewCapsule(math::vec3(0, 1, 0) + math::vec3(0, .5, 0), 1, 0.5f);
            player.AddComponent<ColliderComponent>(hull);
        }
        { // Platform
            float platformSize = 20.0f;
            float platformThickness = 3.0f;

            auto platform = scene->CreateGameObject("Platform");

            auto rectMesh = MeshCatalog::Get("mesh_plane");
            auto material = rectMesh->GetMaterial(0);
            material->Set<float>("u_TextureScale", platformSize);
            material->Set("u_AlbedoTexture", Texture2D::Create("Data/Images/grid/PNG/Dark/texture_07.png"));
            platform.AddComponent<MeshRendererComponent>(rectMesh);

            auto& trans = platform.GetComponent<TransformComponent>().Transform;
            trans = math::mat4();
            trans.scale(math::vec3(platformSize, 1, platformSize));
            trans.translate(math::vec3(0, 0.0f, 0));

            UID_t floor = cWorld.CreateNewCubeHull(math::vec3(0, -platformThickness / 2.0f, 0), 2 * platformSize, platformThickness, 2 * platformSize);
        }

        { // Lights
            auto light = scene->CreateGameObject("Sun");
            light.AddComponent<LightComponent>(LightType::Directional, math::vec3(1.0f, 236.0f / 255.0f, 225.0f / 255.0f), 5, 0, 0);
            auto& trans = light.GetComponent<TransformComponent>().Transform;
            trans = math::createYawPitchRollMatrix(45, 0, -80);
        }

        { // Camera
            math::vec2 viewportSize = {
                (float)Application::Get().GetWindow().GetWidth(),
                (float)Application::Get().GetWindow().GetHeight() };

            auto Camera = scene->CreateGameObject("Camera");
            auto& camera = Camera.AddComponent<CameraComponent>().camera;
            camera.SetViewportSize(viewportSize.x, viewportSize.y);
            camera.SetPerspective(75, .01, 100);
            //Camera.AddComponent<NativeScriptComponent>().Bind<CameraController>(Camera);
            BindGameScript("script_camera_controller", scene, Camera);

            auto& trans = Camera.GetComponent<TransformComponent>().Transform;
            trans = math::mat4();
            trans.translate(math::vec3(0, 4, 5));
            trans *= math::createYawPitchRollMatrix(0, 0, -45);
        }

        // Ramps at various angles
        cWorld.getHullFromID(cWorld.CreateNewCubeHull(math::vec3(5, 0, -5), 10, 1, 3))
            ->rotation.toYawPitchRoll(0, 10, 0);
        cWorld.getHullFromID(cWorld.CreateNewCubeHull(math::vec3(5, 1, -2), 10, 1, 3))
            ->rotation.toYawPitchRoll(0, 20, 0);
        cWorld.getHullFromID(cWorld.CreateNewCubeHull(math::vec3(5, 2, 1), 10, 1, 3))
            ->rotation.toYawPitchRoll(0, 30, 0);
        cWorld.getHullFromID(cWorld.CreateNewCubeHull(math::vec3(5, 3, 4), 10, 1, 3))
            ->rotation.toYawPitchRoll(0, 40, 0);
        cWorld.getHullFromID(cWorld.CreateNewCubeHull(math::vec3(5, 3.5, 7), 10, 1, 3))
            ->rotation.toYawPitchRoll(0, 50, 0);

        // Sound stuff
        {
            SoundEngine::CreateSoundCue("guard_death", { "Data/Sounds/death.ogg", 0.02f });
            SoundEngine::CreateSoundCue("golem", { "Data/Sounds/golem.ogg", 0.1f }); //MONO, has 3D sound
            SoundEngine::CreateSoundCue("protector", { "Data/Sounds/sound.wav", 0.2f });
            SoundEngine::CreateSoundCue("ahhh", { "Data/Sounds/ahhh.ogg", 0.1f, 15.0f });
        }
    }

    void load_level_2(Scene3D* scene) {
        Engine::MeshCatalog::Register("mesh_guard",      "Data/Models/guard.nbt", true);
        Engine::MeshCatalog::Register("mesh_breakroom",  "../../Assets/Blender/Level 1/breakroom.nbt", true);
        Engine::MeshCatalog::Register("mesh_floor",      "../../Assets/Blender/Level 1/floor.nbt", true);
        Engine::MeshCatalog::Register("mesh_backrooms",  "../../Assets/Blender/Level 1/backrooms.nbt", true);
        Engine::MeshCatalog::Register("mesh_outerwalls", "../../Assets/Blender/Level 1/outerwalls.nbt", true);
        Engine::MeshCatalog::Register("mesh_safe",       "../../Assets/Blender/Level 1/safe.nbt", true);
        Engine::MeshCatalog::Register("mesh_saferoom",   "../../Assets/Blender/Level 1/saferoom.nbt", true);
        auto& cWorld = scene->GetCollisionWorld();

        { // Player
            auto player = scene->CreateGameObject("Player");
            auto mesh = MeshCatalog::Get("mesh_guard");
            player.AddComponent<MeshRendererComponent>(mesh);
            BindGameScript("script_player_controller", scene, player);

            mesh->GetSubmeshes()[0].Transform = math::createYawPitchRollMatrix(90, 0, 0);

            auto& trans = player.GetComponent<TransformComponent>().Transform;
            trans = math::mat4();
            trans.translate(math::vec3(-8, 1, 12));

            UID_t hull = cWorld.CreateNewCapsule(math::vec3(-8, 1, 12) + math::vec3(0, .5, 0), 1, 0.5f);
            player.AddComponent<ColliderComponent>(hull);
        }

        std::vector<std::string> level_meshes{
            "mesh_breakroom",
            "mesh_floor",
            "mesh_backrooms",
            "mesh_outerwalls",
            "mesh_safe",
            "mesh_saferoom"
        };

        for (const auto& m : level_meshes) {
            auto go = scene->CreateGameObject(m);
            auto mesh = MeshCatalog::Get(m);
            go.AddComponent<MeshRendererComponent>(mesh);
        }
        UID_t floor = cWorld.CreateNewCubeHull(math::vec3(0, -1.5f, 0), 40, 3, 40);

        std::vector<math::vec3> level_point_lights{
            {-8.0f, 3, 12.0f},
            {-10.0f, 3, 10.0f},
            {-14.75f, 3, 6.25f},
            {-0.5f, 3, 11.75f},
            {-4.0f, 3, 6.75f},
            {5.0f, 3, 10.25f},
            {5.0f, 3, 4.75f},
            {5.0f, 3, -2.0f},
            {-4.25f, 3, 1.75f},
            {-1.75f, 3, 1.75f},
            {-12.0f, 3, -1.0f}
        };

        int n = 0;
        for (const auto& light_pos : level_point_lights) {
            auto light = scene->CreateGameObject("Light " + std::to_string(n));
            light.AddComponent<LightComponent>(LightType::Point, math::vec3(1, 1, 1), 3, 0, 0);
            auto& trans = light.GetComponent<TransformComponent>().Transform;
            trans = math::mat4();
            trans.translate(light_pos);
        }

        { // Lights
            auto light = scene->CreateGameObject("Sun");
            light.AddComponent<LightComponent>(LightType::Directional, math::vec3(.8f, .95f, .9f), 5, 0, 0);
            auto& trans = light.GetComponent<TransformComponent>().Transform;
            trans = math::createYawPitchRollMatrix(15, 0, -80);
        }

        { // Camera
            math::vec2 viewportSize = {
                (float)Application::Get().GetWindow().GetWidth(),
                (float)Application::Get().GetWindow().GetHeight() };

            auto Camera = scene->CreateGameObject("Camera");
            auto& camera = Camera.AddComponent<CameraComponent>().camera;
            camera.SetViewportSize(viewportSize.x, viewportSize.y);
            camera.SetPerspective(75, .01, 100);
            //Camera.AddComponent<NativeScriptComponent>().Bind<CameraController>(Camera);
            BindGameScript("script_camera_controller", scene, Camera);

            auto& trans = Camera.GetComponent<TransformComponent>().Transform;
            trans = math::mat4();
            trans.translate(math::vec3(0, 4, 5));
            trans *= math::createYawPitchRollMatrix(0, 0, -45);
        }
    }

    void load_level_3(Scene3D* scene) {
        // load materials
        std::unordered_map<std::string, md5::Material> mats;
        md5::LoadMD5MaterialDefinitionFile("Data/animTest/bob_lamp_update_export.md5material", mats);
        md5::LoadMD5MaterialDefinitionFile("Data/animTest/tentacle.md5material", mats);
        MaterialCatalog::RegisterMaterial(mats);

        // read md5mesh file
        md5::Model model;
        Engine::md5::LoadMD5MeshFile("Data/animTest/bob_lamp_update_export.md5mesh", &model);
        MeshCatalog::Register("mesh_bob", model);
        Engine::MeshCatalog::Register("mesh_guard", "Data/Models/guard.nbt", true);
        Engine::MeshCatalog::Register("mesh_plane", "Data/Models/plane.nbt", true);
        auto& cWorld = scene->GetCollisionWorld();

        // read animation file
        Ref<md5::Animation> anim = std::make_shared<md5::Animation>();
        md5::LoadMD5AnimFile("Data/animTest/bob_lamp_update_export.md5anim", anim.get());

        //md5::Animation anim;
        // create md5 mesh
        /*
        {
            using namespace math;
            md5::Model tentacle;
            
            md5::Joint joint0;
            joint0.name = "Root";
            joint0.parent = -1;
            joint0.position = vec3(0, 0, 0);
            joint0.orientation = vec4(0, 0, 0, 1);
            md5::Joint joint1;
            joint1.name = "tip";
            joint1.parent = 0;
            joint1.position = vec3(0, 1, 0);
            joint1.orientation = vec4(0, 0, 0, 1);
            tentacle.Joints.push_back(joint0);
            tentacle.Joints.push_back(joint1);
            tentacle.numJoints = 2;

            md5::Mesh mesh;
            mesh.Shader = "tentacle";

            md5::Vert vert;
            vert.countWeight = 1;
            vert.startWeight = 0;
            vert.uv = vec2(0, 0);
            vert.position = vec3(-1, 0, 0);
            vert.normal = vec3(0, 0, 1);
            mesh.Verts.push_back(vert);
            
            vert.startWeight = 1;
            vert.uv = vec2(1, 0);
            vert.position = vec3(1, 0, 0);
            mesh.Verts.push_back(vert);

            vert.startWeight = 2;
            vert.countWeight = 2;
            vert.uv = vec2(.1, .4);
            vert.position = vec3(-.75f, 1, 0);
            mesh.Verts.push_back(vert);

            vert.startWeight = 4;
            vert.countWeight = 2;
            vert.uv = vec2(.9, .4);
            vert.position = vec3(.75f, 1, 0);
            mesh.Verts.push_back(vert);

            vert.startWeight = 6;
            vert.countWeight = 1;
            vert.uv = vec2(.3, .8);
            vert.position = vec3(-.75f, 2, 0);
            mesh.Verts.push_back(vert);

            vert.startWeight = 7;
            vert.countWeight = 1;
            vert.uv = vec2(.7, .8);
            vert.position = vec3(.75f, 2, 0);
            mesh.Verts.push_back(vert);

            vert.startWeight = 8;
            vert.countWeight = 1;
            vert.uv = vec2(.5, 1);
            vert.position = vec3(0.0f, 2.35f, 0);
            mesh.Verts.push_back(vert);

            md5::Tri tri;
            tri.vertIndex[0] = 0;
            tri.vertIndex[1] = 1;
            tri.vertIndex[2] = 3;
            mesh.Tris.push_back(tri);

            tri.vertIndex[0] = 0;
            tri.vertIndex[1] = 3;
            tri.vertIndex[2] = 2;
            mesh.Tris.push_back(tri);

            tri.vertIndex[0] = 2;
            tri.vertIndex[1] = 3;
            tri.vertIndex[2] = 5;
            mesh.Tris.push_back(tri);

            tri.vertIndex[0] = 2;
            tri.vertIndex[1] = 5;
            tri.vertIndex[2] = 4;
            mesh.Tris.push_back(tri);

            tri.vertIndex[0] = 4;
            tri.vertIndex[1] = 5;
            tri.vertIndex[2] = 6;
            mesh.Tris.push_back(tri);

            md5::Weight weight;
            weight.weightIndex = 0;
            weight.joint = 0;
            weight.bias = 1.0f;
            mesh.Weights.push_back(weight);

            weight.weightIndex = 1;
            weight.joint = 0;
            weight.bias = 1.0f;
            mesh.Weights.push_back(weight);

            weight.weightIndex = 2;
            weight.joint = 0;
            weight.bias = 0.5f;
            mesh.Weights.push_back(weight);

            weight.weightIndex = 3;
            weight.joint = 1;
            weight.bias = 0.5f;
            mesh.Weights.push_back(weight);

            weight.weightIndex = 4;
            weight.joint = 0;
            weight.bias = 0.5f;
            mesh.Weights.push_back(weight);

            weight.weightIndex = 5;
            weight.joint = 1;
            weight.bias = 0.5f;
            mesh.Weights.push_back(weight);

            weight.weightIndex = 6;
            weight.joint = 1;
            weight.bias = 1.0f;
            mesh.Weights.push_back(weight);

            weight.weightIndex = 7;
            weight.joint = 1;
            weight.bias = 1.0f;
            mesh.Weights.push_back(weight);

            weight.weightIndex = 8;
            weight.joint = 1;
            weight.bias = 1.0f;
            mesh.Weights.push_back(weight);

            tentacle.Meshes.push_back(mesh);
            tentacle.numMeshes = 1;
            MeshCatalog::Register("mesh_tentacle", tentacle);

            // Animation
            anim->numFrames = 4;
            anim->numJoints = 2;
            anim->numAnimComponents = 12;
            anim->FrameRate = 1;
            anim->frameDuration = 1.0f / (float)anim->FrameRate;
            anim->animDuration = anim->frameDuration * (float)anim->numFrames;
            anim->animTime = 0.0f;

            md5::JointInfo joint;
            joint.name = "Root";
            joint.parentID = -1;
            joint.flags = 1 | 2 | 4 | 8 | 16 | 32;
            joint.startIndex = 0;
            anim->JointInfos.push_back(joint);
            joint.name = "tip";
            joint.parentID = 0;
            joint.flags = 1 | 2 | 4 | 8 | 16 | 32;
            joint.startIndex = 6;
            anim->JointInfos.push_back(joint);

            md5::BaseFrame bf;
            bf.position = vec3(0, 0, 0);
            bf.orientation = vec4(0, 0, 0, 1);
            anim->BaseFrames.push_back(bf);
            bf.position = vec3(0, 1, 0);
            bf.orientation = vec4(0, 0, 0, 1);
            anim->BaseFrames.push_back(bf);

            md5::FrameData frame;
            frame.frameID = 0;
            frame.frameData.push_back(-1); // bone 0 pos
            frame.frameData.push_back(0);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 0 rot
            frame.frameData.push_back(0);
            frame.frameData.push_back(.05);
            frame.frameData.push_back(0); // bone 1 pos
            frame.frameData.push_back(1);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 1 rot
            frame.frameData.push_back(0);
            frame.frameData.push_back(.05);
            anim->Frames.push_back(frame);

            md5::BuildFrameSkeleton(anim->Skeletons, anim->JointInfos, anim->BaseFrames, frame);

            frame.frameID = 1;
            frame.frameData.clear();
            frame.frameData.push_back(0); // bone 0 pos
            frame.frameData.push_back(0);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 0 rot
            frame.frameData.push_back(0);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 1 pos
            frame.frameData.push_back(1);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 1 rot
            frame.frameData.push_back(0);
            frame.frameData.push_back(0);
            anim->Frames.push_back(frame);

            md5::BuildFrameSkeleton(anim->Skeletons, anim->JointInfos, anim->BaseFrames, frame);

            frame.frameID = 2;
            frame.frameData.clear();
            frame.frameData.push_back(1); // bone 0 pos
            frame.frameData.push_back(0);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 0 rot
            frame.frameData.push_back(0);
            frame.frameData.push_back(-.05);
            frame.frameData.push_back(0); // bone 1 pos
            frame.frameData.push_back(1);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 1 rot
            frame.frameData.push_back(0);
            frame.frameData.push_back(-.05);
            anim->Frames.push_back(frame);

            md5::BuildFrameSkeleton(anim->Skeletons, anim->JointInfos, anim->BaseFrames, frame);

            frame.frameID = 3;
            frame.frameData.clear();
            frame.frameData.push_back(0); // bone 0 pos
            frame.frameData.push_back(0);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 0 rot
            frame.frameData.push_back(0);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 1 pos
            frame.frameData.push_back(1);
            frame.frameData.push_back(0);
            frame.frameData.push_back(0); // bone 1 rot
            frame.frameData.push_back(0);
            frame.frameData.push_back(0);
            anim->Frames.push_back(frame);

            md5::BuildFrameSkeleton(anim->Skeletons, anim->JointInfos, anim->BaseFrames, frame);

            anim->AnimatedSkeleton.Joints.assign(anim->numJoints, md5::SkeletonJoint());
        }
        */

        { // Player
            auto player = scene->CreateGameObject("Player");
            auto mesh = MeshCatalog::Get("mesh_bob");
            player.AddComponent<MeshRendererComponent>(mesh);
            player.AddComponent<MeshAnimationComponent>(anim);
            BindGameScript("script_player_controller", scene, player);

            auto& trans = player.GetComponent<TransformComponent>().Transform;
            trans = math::mat4();
            trans.translate(math::vec3(0, 1, 0));

            UID_t hull = cWorld.CreateNewCapsule(math::vec3(0, 1, 0) + math::vec3(0, .5, 0), 1, 0.5f);
            player.AddComponent<ColliderComponent>(hull);
        }

        { // Platform
            float platformSize = 20.0f;
            float platformThickness = 3.0f;

            auto platform = scene->CreateGameObject("Platform");

            auto rectMesh = MeshCatalog::Get("mesh_plane");
            auto material = rectMesh->GetMaterial(0);
            material->Set<float>("u_TextureScale", platformSize);
            material->Set("u_AlbedoTexture", Texture2D::Create("Data/Images/grid/PNG/Dark/texture_07.png"));
            //platform.AddComponent<MeshRendererComponent>(rectMesh);

            auto& trans = platform.GetComponent<TransformComponent>().Transform;
            trans = math::mat4();
            trans.scale(math::vec3(platformSize, 1, platformSize));
            trans.translate(math::vec3(0, 0.0f, 0));

            UID_t floor = cWorld.CreateNewCubeHull(math::vec3(0, -platformThickness / 2.0f, 0), 2 * platformSize, platformThickness, 2 * platformSize);
        }

        { // Lights
            auto light = scene->CreateGameObject("Sun");
            light.AddComponent<LightComponent>(LightType::Directional, math::vec3(1.0f, 236.0f / 255.0f, 225.0f / 255.0f), 5, 0, 0);
            auto& trans = light.GetComponent<TransformComponent>().Transform;
            trans = math::createYawPitchRollMatrix(45, 0, -80);
        }

        { // Camera
            math::vec2 viewportSize = {
                (float)Application::Get().GetWindow().GetWidth(),
                (float)Application::Get().GetWindow().GetHeight() };

            auto Camera = scene->CreateGameObject("Camera");
            auto& camera = Camera.AddComponent<CameraComponent>().camera;
            camera.SetViewportSize(viewportSize.x, viewportSize.y);
            camera.SetPerspective(75, .01, 100);
            BindGameScript("script_camera_controller", scene, Camera);

            auto& trans = Camera.GetComponent<TransformComponent>().Transform;
            trans = math::mat4();
            trans.translate(math::vec3(0, 4, 5));
            trans *= math::createYawPitchRollMatrix(0, 0, -45);
        }
    }
}