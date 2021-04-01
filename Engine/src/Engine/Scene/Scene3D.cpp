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
        GameObject go = { m_Registry.Create(), this };
        go.AddComponent<TransformComponent>();
        go.AddComponent<TagComponent>(name.empty() ? "GameObject" : name);
        return go;
    }

    GameObject Scene3D::FindByName(const std::string& name) {
        // TODO: why can't I just get all entities that have tags, and return the dameOgject id then?
        // since all GameObjects created are given a tag by default, this might not be bad...
        //const auto& entities = m_Registry.GetRegList();
        for (const auto& ent : m_Registry.GetRegList()) {
            if (m_Registry.has<TagComponent>(ent)) {
                const auto& tag_name = m_Registry.get<TagComponent>(ent).Name;
                if (tag_name.compare(name) == 0) {
                    return GameObject(ent, this);
                }
            }
        }

        return GameObject(0, nullptr);
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
            auto& scriptComponents = m_Registry.view<NativeScriptComponent>();
            for (auto& script : scriptComponents) {
                if (!script.Script) {
                    script.Script = script.InstantiateScript();
                    script.Script->m_GameObject = { script.GameObjectID, this };
                    script.Script->OnCreate();

                    ENGINE_LOG_ASSERT(script.Script, "Native script not instantiated");
                }
            }

            SoundEngine::StartSteam();
        }

        m_Playing = true;
    }

    void Scene3D::OnRuntimePause() {
        m_Playing = false;
    }

    void Scene3D::OnRuntimeResume() {
        m_Playing = true;
    }

    void Scene3D::OnRuntimeStop() {
        BENCHMARK_FUNCTION();
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

    void Scene3D::OnUpdate(double dt) {
        BENCHMARK_FUNCTION();
        if (m_Playing) {
            BENCHMARK_SCOPE("Update Scripts");
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
            BENCHMARK_SCOPE("Find main camera");
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
            BENCHMARK_SCOPE("Determine lights");
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
            BENCHMARK_SCOPE("Render");
            Renderer::Begin3DScene(*mainCamera, *mainTransform, numPointLight, scenePointLights, numSpotLight, sceneSpotLights, sceneSun);

            Renderer::BeginDeferredPrepass();
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
            Renderer::EndDeferredPrepass();

            Renderer::BeginSobelPass();
            // Render collision hulls
            if (m_showCollisionHulls) {
                for (const auto& hull : m_cWorld.m_static) {
                    math::mat4 transform;
                    transform.translate(hull.position);
                    transform *= math::mat4(hull.rotation);
                    Renderer::Submit(hull.wireframe, transform, math::vec3(1, .05, .1));
                }
                for (const auto& hull : m_cWorld.m_dynamic) {
                    math::mat4 transform;
                    transform.translate(hull.position);
                    transform *= math::mat4(hull.rotation);
                    Renderer::Submit(hull.wireframe, transform, math::vec3(.1, .05, 1));
                }
            }
            Renderer::EndSobelPass();

            Renderer::End3DScene();

            Renderer::RenderDebugUI();
            if (m_showEntityLocations) { // TODO: rename this/come up with less bad solution for these things
                TextRenderer::SubmitText("Showing entity locations", 5, 40, math::vec3(.7, .1, .5));
            }
        }
        
    }

    void Scene3D::OnViewportResize(u32 width, u32 height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        auto& cameraComponentList = m_Registry.view<CameraComponent>();
        for (auto& cameraComponent : cameraComponentList) {
            if (!cameraComponent.FixedAspectRatio) {
                cameraComponent.camera.SetViewportSize(width, height);
            }
        }
    }

    Scene3D::Scene3D() {
    }

    Scene3D::~Scene3D() {
    }
}