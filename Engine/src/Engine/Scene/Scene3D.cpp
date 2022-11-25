#include <enpch.hpp>
#include "Scene3D.hpp"

#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/TextRenderer.hpp"
#include "Engine/GameObject/Components.hpp"

#include "Engine/Resources/nbt/nbt.hpp"
#include "Engine/Resources/MeshCatalog.hpp"
#include "Engine/Sound/SoundEngine.hpp"

// hard-coded levels
#include "Engine/Examples/ExampleLevels.hpp"

namespace Engine {

    GameObject Scene3D::CreateGameObject(const std::string& name) {
        GameObject go = { m_Registry.create(), this };
        go.AddComponent<TransformComponent>();
        go.AddComponent<TagComponent>(name.empty() ? " " : name);
        return go;
    }

    GameObject Scene3D::FindByName(const std::string& name) {
        // might be a better way of doing this in entt
        auto view = m_Registry.view<TagComponent>();
        for (auto entity : view) {
            auto &tag = view.get<TagComponent>(entity);
            const auto& tag_name = tag.Name;
            if (tag_name.compare(name) == 0) {
                return GameObject(entity, this);
            }
        }

        return GameObject(entt::null, nullptr);
    }

    bool Scene3D::loadFromLevel(const std::string& levelName) {
        BENCHMARK_FUNCTION();
        std::string filename = "Data/Levels/" + levelName + ".scene";

        // TEMP
        // First check if it is one of the hard-coded levels
        if (LoadExampleLevel(levelName, this)) {
            return true;
        }

        return false;
    }

    void Scene3D::Destroy() {
        OnRuntimeStop();

        //m_Registry.clear();
    }

    void Scene3D::OnRuntimeStart() {
        BENCHMARK_FUNCTION();
        if (!m_Playing) {
            // Initialize all scripts
            std::vector<ScriptableBase*> inited_scripts;
            auto script_view = m_Registry.view<NativeScriptComponent>();
            for (auto entity : script_view) {
                auto &script = script_view.get< NativeScriptComponent>(entity);

                if (!script.Script) {
                    script.Script = script.InstantiateScript();
                    script.Script->m_GameObject = { script.GameObjectID, this };
                    script.Script->OnCreate();

                    inited_scripts.push_back(script.Script);

                    ENGINE_LOG_ASSERT(script.Script, "Native script not instantiated");
                }
            }
            // Now that all scripts are initialized, link them together (if they need)
            for (auto script : inited_scripts) {
                script->OnLink();
            }

            SoundEngine::StartStream();
        }

        m_Playing = true;
    }

    void Scene3D::OnRuntimePause() {
        SoundEngine::PauseStream();

        m_Playing = false;
    }

    void Scene3D::OnRuntimeResume() {
        SoundEngine::ResumeStream();

        m_Playing = true;
    }

    void Scene3D::OnRuntimeStop() {
        SoundEngine::StopStream();

        BENCHMARK_FUNCTION();
        if (m_Playing) {
            // Destroy all scripts
            auto script_view = m_Registry.view<NativeScriptComponent>();
            for (auto entity : script_view) {
                auto &script = script_view.get< NativeScriptComponent>(entity);

                if (script.Script) {
                    script.DestroyScript(&script);

                    ENGINE_LOG_ASSERT(!script.Script, "Native script not destroyed");
                }
            }
        }

        m_Playing = false;
    }

    void Scene3D::OnUpdate(double dt) {
        BENCHMARK_FUNCTION();
        if (m_Playing) {
            BENCHMARK_SCOPE("Update Scripts");
            // Update scripts
            auto script_view = m_Registry.view<NativeScriptComponent>();
            for (auto entity : script_view) {
                auto &script = script_view.get< NativeScriptComponent>(entity);

                if (script.Script) {
                    ENGINE_LOG_ASSERT(script.Script, "Native script not instantiated");

                    script.Script->OnUpdate(dt);
                }
            }

            // Loop through all animated meshes and update their animations
            auto mesh_view = m_Registry.view<MeshRendererComponent>();
            for (auto entity : mesh_view) {
                auto &mesh = mesh_view.get<MeshRendererComponent>(entity);

                if (mesh.MeshPtr) {
                    mesh.MeshPtr->OnUpdate(dt);
                }
            }
        }

        // Find Main Camera
        Camera* mainCamera = nullptr;
        math::mat4* mainTransform = nullptr;
        {
            BENCHMARK_SCOPE("Find main camera");
            auto group = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
            for (auto entity : group) {
                auto[trans, cam] = group.get<TransformComponent, CameraComponent>(entity);

                if (cam.Primary) {
                    mainCamera = &cam.camera;
                    mainTransform = &trans.Transform;
                    break;
                }
            }
        }

        // Get point lights
        Light scenePointLights[32]; // TODO: where is this number stored?
        Light sceneSpotLights[32]; // TODO: where is this number stored?
        Light sceneSun;
        int numPointLight = 0, numSpotLight = 0;
        {
            BENCHMARK_SCOPE("Determine lights");
            
            auto group = m_Registry.group<LightComponent>(entt::get<TransformComponent>);
            for (auto entity : group) {
                auto[trans, light] = group.get<TransformComponent, LightComponent>(entity);

                switch (light.Type) {
                case LightType::Point:
                    if (numPointLight == 32) break;
                    scenePointLights[numPointLight].color = light.Color;
                    scenePointLights[numPointLight].strength = light.Strength;
                    scenePointLights[numPointLight].type = light.Type;
                    scenePointLights[numPointLight].position = trans.Transform.column4.asVec3();
                    numPointLight++;
                    break;
                case LightType::Spot:
                    if (numSpotLight == 32) break;
                    sceneSpotLights[numSpotLight].color = light.Color;
                    sceneSpotLights[numSpotLight].strength = light.Strength;
                    sceneSpotLights[numSpotLight].type = light.Type;
                    sceneSpotLights[numSpotLight].position = trans.Transform.column4.asVec3();
                    sceneSpotLights[numSpotLight].direction = -trans.Transform.column3.asVec3();
                    sceneSpotLights[numSpotLight].inner = light.InnerCutoff;
                    sceneSpotLights[numSpotLight].outer = light.OuterCutoff;
                    numSpotLight++;
                    break;
                case LightType::Directional:
                    sceneSun.color = light.Color;
                    sceneSun.strength = light.Strength;
                    sceneSun.type = light.Type;
                    sceneSun.position = trans.Transform.column4.asVec3();
                    sceneSun.direction = -trans.Transform.column3.asVec3(); // sun points in entities -z direction (or whatever the camera looks in...)
                    break;
                }
            }
        }

        // Render
        if (mainCamera) {
            BENCHMARK_SCOPE("Render");
            Renderer::Begin3DScene(*mainCamera, *mainTransform, numPointLight, scenePointLights, numSpotLight, sceneSpotLights, sceneSun);

            Renderer::BeginDeferredPrepass();

            auto group = m_Registry.group<MeshRendererComponent>(entt::get<TransformComponent>);
            for (auto entity : group) {
                auto[trans, mesh] = group.get<TransformComponent, MeshRendererComponent>(entity);
                if (mesh.MeshPtr) {                    
                    auto& name_str = m_Registry.get<TagComponent>(entity).Name;
                    Renderer::SubmitMesh(mesh.MeshPtr, trans.Transform);
                }
            }

            Renderer::EndDeferredPrepass();

            Renderer::BeginSobelPass();
            // Render collision hulls
            if (m_showCollisionHulls) {
                for (const auto& hull : m_cWorld.m_static) {
                    math::mat4 transform;
                    math::CreateTransform(transform, hull.rotation, hull.position);
                    Renderer::Submit(hull.wireframe, transform, math::vec3(1, .05, .1));
                }
                for (const auto& hull : m_cWorld.m_dynamic) {
                    math::mat4 transform;
                    math::CreateTransform(transform, hull.rotation, hull.position);
                    Renderer::Submit(hull.wireframe, transform, math::vec3(.1, .05, 1));
                }
            }
            Renderer::EndSobelPass();

            Renderer::End3DScene();

            Renderer::RenderDebugUI();
            if (m_showEntityLocations) { // TODO: rename this/come up with less bad solution for these things
                TextRenderer::SubmitText("Showing entity locations", 5, 40, math::vec3(.7, .1, .5));
            }

            // draw coordinate frames
            Renderer::SubmitLine(math::vec3(), math::vec3(1, 0, 0), math::vec4(1, 0, 0, 1));
            Renderer::SubmitLine(math::vec3(), math::vec3(0, 1, 0), math::vec4(0, 1, 0, 1));
            Renderer::SubmitLine(math::vec3(), math::vec3(0, 0, 1), math::vec4(0, 0, 1, 1));

            auto axes_view = m_Registry.view<TransformComponent>();
            for (auto entity : axes_view) {
                auto& transform = axes_view.get<TransformComponent>(entity);

                math::vec3 pos = transform.Transform.column4.asVec3();
                math::vec3 forward, right, up;
                math::Decompose(transform.Transform, forward, right, up);

                // right   +X
                // up      +Y
                // forward -Z
                Renderer::SubmitLine(pos, pos+right, math::vec4(1, 0, 0, 1));
                Renderer::SubmitLine(pos, pos+up, math::vec4(0, 1, 0, 1));
                Renderer::SubmitLine(pos, pos+forward, math::vec4(0, 0, 1, 1));
            }

            // Draw skeletons
            auto anim_group = m_Registry.group<MeshRendererComponent>(entt::get<TransformComponent>);
            for (auto entity : anim_group) {
                auto[transform, mesh] = anim_group.get<TransformComponent, MeshRendererComponent>(entity);
                if (mesh.MeshPtr) {
                    auto& tag = m_Registry.get<TagComponent>(entity);
                    
                    if (mesh.MeshPtr->HasAnimations()) {
                        Renderer::DrawSkeletonDebug(tag, transform, mesh, math::vec3(.6f, .1f, .9f));
                    }
                }
            }

            auto group_trans = m_Registry.group<TransformComponent>(entt::get<TagComponent>);
            for (auto entity : group_trans) {
                const auto& tag = group_trans.get<TagComponent>(entity);
                const auto& transform = group_trans.get<TransformComponent>(entity);
                math::vec3 pos = transform.Transform.column4.asVec3();
                Renderer::Draw3DText(tag.Name, pos, math::vec3(.7f, .7f, 1.0f));
            }
        }
        
    }

    void Scene3D::OnViewportResize(u32 width, u32 height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            auto &cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio) {
                cameraComponent.camera.SetViewportSize(width, height);
            }
        }
    }

    Scene3D::Scene3D() {
        // reserve gameObject 0
        //m_Registry.create();
    }

    Scene3D::~Scene3D() {
    }
}