#include <enpch.hpp>
#include "Scene.hpp"

#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/TextRenderer.hpp"
#include "Engine/GameObject/Components.hpp"

#include "Engine/Resources/nbt/nbt.hpp"
#include "Engine/Resources/MeshCatalog.hpp"

namespace Engine {

    GameObject Scene::CreateGameObject(const std::string& name) {
        GameObject go = { m_Registry.Create(), this };
        go.AddComponent<TransformComponent>();
        go.AddComponent<TagComponent>(name.empty() ? "GameObject" : name);
        return go;
    }

    void Scene::loadFromFile(const std::string& filename) {
        /*
        
        1. read all strings
        2. create all gameobjects
        3. attach all components

        */

        nbt::file_data data;
        nbt::nbt_byte version_major, version_minor;
        endian::endian endianness;
        ENGINE_LOG_INFO("Loading .nbt level file from \"{0}\"", filename);
        bool success = nbt::read_from_file(filename, data, version_major, version_minor, endianness);

        if (!success) {
            ENGINE_LOG_ERROR("Failed to load /nbt level file \"{0}\"", filename);
            return;
        }

        auto str = data.first;
        auto& comp = data.second->as<nbt::tag_compound>();

        /* Extract global scene data */
        auto level_name = comp["name"].as<nbt::tag_string>().get();
        ENGINE_LOG_INFO("Level name: {0}", level_name);

        /* First get all the meshes that need to be loaded */
        auto mesh_list = comp["meshes"].as<nbt::tag_list>();
        ENGINE_LOG_ASSERT(mesh_list.el_type() == nbt::tag_type::Compound, "Mesh list needs to be a lag_list of tag_compound!");

        for (auto& mesh : mesh_list) {
            auto& tc = mesh.as<nbt::tag_compound>();

            auto mesh_name = tc["mesh_name"].as<nbt::tag_string>();
            auto mesh_path = tc["mesh_path"].as<nbt::tag_string>();
            bool is_nbt = tc.has_key("nbt");
            
            MeshCatalog::Register(mesh_name, mesh_path, is_nbt);
        }

        /* loop through entities and create them */
        auto entity_list = comp["entities"].as<nbt::tag_list>();
        ENGINE_LOG_ASSERT(entity_list.el_type() == nbt::tag_type::Compound, "Entity list needs to be a lag_list of tag_compound!");

        for (auto& entity : entity_list) {
            auto& tc = entity.as<nbt::tag_compound>();

            auto& name = tc["name"].as<nbt::tag_string>();
            // create the entity
            ENGINE_LOG_TRACE("Entity {0}", name);
            auto go = CreateGameObject(name);

            auto& transform = tc["transform"].as<nbt::tag_compound>();
            math::vec3 position, scale(1,1,1);
            math::vec4 rotation;
            if (transform.has_key("position"))
                position = transform["position"].as<nbt::tag_vec3>().get();
            if (transform.has_key("rotation"))
                rotation = transform["rotation"].as<nbt::tag_vec4>().get();
            if (transform.has_key("scale"))
                scale = transform["scale"].as<nbt::tag_vec3>().get();

            // TODO: allow for rotations (need quaternion to yaw/pitch/roll
            // TODO: alternatively, ned function to build transform from position/scale vectors and rotation quaternion
            auto& T = go.GetComponent<TransformComponent>().Transform;
            T = math::mat4();
            T.translate(position);
            //auto q = math::quat2ypr(rotation);
            T *= math::createYawPitchRollMatrix(0, 0, 0);
            T *= math::mat4(scale.x, scale.y, scale.z, 1.0f);

            auto& component_list = tc["components"].as<nbt::tag_list>();
            ENGINE_LOG_ASSERT(component_list.el_type() == nbt::tag_type::Compound, "Component list needs to be a lag_list of tag_compound!");

            for (auto& component : component_list) {
                auto& comp_type = component["type"].as<nbt::tag_string>().get();

                if (comp_type.compare("MeshRenderer") == 0) {
                    ENGINE_LOG_INFO("adding a MeshRenderer component");

                    auto& mesh_name = component["mesh_name"].as<nbt::tag_string>().get();
                    auto __mesh = MeshCatalog::Get(mesh_name);
                    go.AddComponent<MeshRendererComponent>(__mesh); // TODO: add blockchain
                } else if (comp_type.compare("Camera") == 0) {
                    ENGINE_LOG_INFO("adding a Camera component");

                    go.AddComponent<CameraComponent>();
                } else if (comp_type.compare("NativeScript") == 0) {
                    ENGINE_LOG_INFO("adding a NativeScript component");

                    auto& script_tag = component["script_tag"].as<nbt::tag_string>().get();
                    if (script_tag.compare("script_builtin_example") == 0) {
                        ENGINE_LOG_ERROR("This is nonsnse temporay code");
                    }
                    else if (Engine::BindGameScript(script_tag, this, go)) {
                        //...
                    }
                    else {
                        ENGINE_LOG_WARN("unknown native script component: {0}", script_tag);
                    }
                }
                else {
                    ENGINE_LOG_WARN("unknown component type!");
                }
            }
        }
    }

    void Scene::writeToFile(const std::string& filename) {
        std::ofstream fp(filename, std::ios::out | std::ios::binary);

        //fp << "Start" << std::endl;

        //fp << "BlockTable";
        //u8 numBlocks = 8;
        //fp.write(reinterpret_cast<char*>(&numBlocks), sizeof(numBlocks));

        auto gameObjects = m_Registry.GetRegList();

        for (auto go : gameObjects) {
            fp << "[GameObject] " << go << std::endl;
            if (m_Registry.has<TagComponent>(go)) {
                fp << "  [TagComponent] " << m_Registry.get<TagComponent>(go).Name << std::endl;
            }
            if (m_Registry.has<MeshRendererComponent>(go)) {
                fp << "  [MeshRendererComponent] " << m_Registry.get<MeshRendererComponent>(go).Mesh->GetFilePath() << std::endl;
            }
            if (m_Registry.has<TransformComponent>(go)) {
                fp << "  [TransformComponent] " <<
                    m_Registry.get<TransformComponent>(go).Transform._11 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._12 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._13 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._14 << " " <<

                    m_Registry.get<TransformComponent>(go).Transform._21 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._22 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._23 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._24 << " " <<

                    m_Registry.get<TransformComponent>(go).Transform._31 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._32 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._33 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._34 << " " <<

                    m_Registry.get<TransformComponent>(go).Transform._41 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._42 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._43 << " " <<
                    m_Registry.get<TransformComponent>(go).Transform._44 << std::endl;
            }
            if (m_Registry.has<CameraComponent>(go)) {
                fp << "  [CameraComponent] " << m_Registry.get<CameraComponent>(go).camera.GetPerspectiveFoV() << std::endl;
            }
            if (m_Registry.has<NativeScriptComponent>(go)) {
                fp << "  [NativeScriptComponent] " << "scriptType" << std::endl;
            }
        }

        //fp << "End" << std::endl;

        fp.close();
    }

    void Scene::OnRuntimeStart() {
        if (!m_Playing) {
            // Initialize all scripts
            auto& scriptComponents = m_Registry.view<NativeScriptComponent>();
            for (auto& script : scriptComponents) {
                if (!script.Script) {
                    script.Script = script.InstantiateScript();
                    script.Script->m_GameObject = { script.GameObjectID, this };
                    script.Script->OnCreate();

                    ENGINE_LOG_ASSERT(script.Script, "Native script not instantiated");
                }
            }
        }

        m_Playing = true;
    }

    void Scene::OnRuntimePause() {
        m_Playing = false;
    }

    void Scene::OnRuntimeResume() {
        m_Playing = true;
    }

    void Scene::OnRuntimeStop() {
        if (m_Playing) {
            // Destroy all scripts
            auto& scriptComponents = m_Registry.view<NativeScriptComponent>();
            for (auto& script : scriptComponents) {
                if (script.Script) {
                    script.DestroyScript(&script);

                    ENGINE_LOG_ASSERT(!script.Script, "Native script not destroyed");
                }
            }
        }

        m_Playing = false;
    }

    void Scene::OnUpdate(double dt) {
        if (m_Playing) {
            // Update
            auto& scriptComponents = m_Registry.view<NativeScriptComponent>();
            for (auto& script : scriptComponents) {
                if (script.Script) {
                    ENGINE_LOG_ASSERT(script.Script, "Native script not instantiated");

                    script.Script->OnUpdate(dt);
                }
            }
        }

        // Find Main Camera
        Camera* mainCamera = nullptr;
        math::mat4* mainTransform = nullptr;
        {
            // messy group
            // TODO: figure out how to get the group function to work
            for (auto n : m_Registry.GetRegList()) {
                // check if entity n has both transform and camera
                if (m_Registry.has<TransformComponent>(n) && m_Registry.has<CameraComponent>(n)) {
                    // good to go
                    auto& cam = m_Registry.get<CameraComponent>(n);
                    auto& tran = m_Registry.get<TransformComponent>(n);

                    if (cam.Primary) {
                        mainCamera = &cam.camera;
                        mainTransform = &tran.Transform;
                        break;
                    }
                }
            }
        }

        // Get point lights
        Light scenePointLights[32]; // TODO: where is this number stored?
        Light sceneSpotLights[32]; // TODO: where is this number stored?
        Light sceneSun;
        int numPointLight = 0, numSpotLight = 0;
        {
            // messy group
            // TODO: figure out how to get the group function to work
            for (auto n : m_Registry.GetRegList()) {
                // check if entity n has both transform and mesh
                if (m_Registry.has<TransformComponent>(n) && m_Registry.has<LightComponent>(n)) {
                    // good to go
                    auto& light = m_Registry.get<LightComponent>(n);
                    auto& trans = m_Registry.get<TransformComponent>(n);
                    auto& tag = m_Registry.get<TagComponent>(n);

                    switch (light.Type) {
                        case LightType::Point:
                            if (numPointLight == 32) break;
                            scenePointLights[numPointLight].color = light.Color;
                            scenePointLights[numPointLight].strength = light.Strength;
                            scenePointLights[numPointLight].type = light.Type;
                            scenePointLights[numPointLight].position = trans.Transform.col4().XYZ();
                            numPointLight++;
                            break;
                        case LightType::Spot:
                            if (numSpotLight == 32) break;
                            sceneSpotLights[numSpotLight].color = light.Color;
                            sceneSpotLights[numSpotLight].strength = light.Strength;
                            sceneSpotLights[numSpotLight].type = light.Type;
                            sceneSpotLights[numSpotLight].position = trans.Transform.col4().XYZ();
                            sceneSpotLights[numSpotLight].direction = -trans.Transform.col3().XYZ();
                            sceneSpotLights[numSpotLight].inner = light.InnerCutoff;
                            sceneSpotLights[numSpotLight].outer = light.OuterCutoff;
                            numSpotLight++;
                            break;
                        case LightType::Directional:
                            sceneSun.color = light.Color;
                            sceneSun.strength = light.Strength;
                            sceneSun.type = light.Type;
                            sceneSun.position = trans.Transform.col4().XYZ();
                            sceneSun.direction = -trans.Transform.col3().XYZ(); // sun points in entities -z direction (or whatever the camera looks in...)
                            break;
                    }
                }
            }
        }

        // Render
        if (mainCamera) {
            Renderer::Begin3DScene(*mainCamera, *mainTransform, numPointLight, scenePointLights, numSpotLight, sceneSpotLights, sceneSun);

            // messy group
            // TODO: figure out how to get the group function to work
            for (auto n : m_Registry.GetRegList()) {
                // check if entity n has both transform and mesh
                if (m_Registry.has<TransformComponent>(n) && m_Registry.has<MeshRendererComponent>(n)) {
                    // good to go
                    auto& mesh = m_Registry.get<MeshRendererComponent>(n);
                    auto& trans = m_Registry.get<TransformComponent>(n);
                    auto& tag = m_Registry.get<TagComponent>(n);

                    if (mesh.Mesh) {
                        Renderer::SubmitMesh(mesh.Mesh, trans.Transform);
                        //if (m_showNormals) // should probably be in a separate part...
                        //    Renderer::SubmitMesh_drawNormals(mesh.Mesh, trans.Transform);
                    }
                }
            }
            Renderer::End3DScene();
        }

        TextRenderer::BeginTextRendering();
        if (m_showEntityLocations) {
            TextRenderer::SubmitText("Showing entity locations", 5, 5, 32, math::vec3(.7, .1, .5));
        }
        TextRenderer::EndTextRendering();
    }

    void Scene::OnViewportResize(u32 width, u32 height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        auto& cameraComponentList = m_Registry.view<CameraComponent>();
        for (auto& cameraComponent : cameraComponentList) {
            if (!cameraComponent.FixedAspectRatio) {
                cameraComponent.camera.SetViewportSize(width, height);
            }
        }
    }

    Scene::Scene() {
    }

    Scene::~Scene() {
    }
}