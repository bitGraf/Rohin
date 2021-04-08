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
            // Update scripts
            auto& scriptComponents = m_Registry.view<NativeScriptComponent>();
            for (auto& script : scriptComponents) {
                if (script.Script) {
                    ENGINE_LOG_ASSERT(script.Script, "Native script not instantiated");

                    script.Script->OnUpdate(dt);
                }
            }

            // Update Animations
            auto& animComponents = m_Registry.view<MeshAnimationComponent>();
            for (auto& anim : animComponents) {
                if (anim.Anim) {
                    md5::UpdateMD5Animation(anim.Anim.get(), dt);
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
                        if (m_Registry.has<MeshAnimationComponent>(n)) {
                            const auto& anim = m_Registry.get<MeshAnimationComponent>(n);
                            Renderer::SubmitMesh(mesh.Mesh, trans.Transform, anim.Anim);
                        }
                        else {
                            Renderer::SubmitMesh(mesh.Mesh, trans.Transform);
                        }
                    }
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

            for (auto n : m_Registry.GetRegList()) {
                // check if entity n has both transform and mesh
                if (m_Registry.has<TransformComponent>(n) && m_Registry.has<MeshAnimationComponent>(n)) {
                    // good to go
                    auto& anim = m_Registry.get<MeshAnimationComponent>(n);
                    auto& trans = m_Registry.get<TransformComponent>(n);
                    auto& tag = m_Registry.get<TagComponent>(n);
                    auto& mesh = m_Registry.get<MeshRendererComponent>(n);

                    std::vector<md5::Joint> baseframe;
                    for (int k = 0; k < anim.Anim->JointInfos.size(); k++) {
                        const auto& joint = anim.Anim->BaseFrames[k];
                        md5::Joint newJoint;
                        newJoint.position = joint.position;
                        newJoint.orientation = joint.orientation;

                        if (anim.Anim->JointInfos[k].parentID >= 0) {
                            const auto& parentJoint = anim.Anim->BaseFrames[anim.Anim->JointInfos[n].parentID];
                            math::vec3 rotPos = math::TransformPointByQuaternion(parentJoint.orientation, newJoint.position);

                            newJoint.position = parentJoint.position + rotPos;
                            newJoint.orientation = parentJoint.orientation * newJoint.orientation;

                            newJoint.orientation.normalize();
                        }
                        baseframe.push_back(newJoint);
                    }

                    //const auto& skeleton = mesh.Mesh->GetBindPose();
                    const auto& skeleton = anim.Anim->AnimatedSkeleton.Joints;
                    //const auto& skeleton = anim.Anim->BaseFrames;
                    //const auto& skeleton = baseframe;
                    //const auto& skeleton1 = anim.Anim->Skeletons[0].Joints;
                    std::vector<math::vec4> colors = {
                        {1,0,1,1},
                    };
                    int c = 0;

                    math::mat3 rotM;
                    math::mat3 scaleM;
                    math::CreateRotationFromYawPitch(rotM, 180, -90);
                    math::CreateScale(scaleM, .3f, .3f, .3f);
                    math::mat3 m = rotM * scaleM;
                    float s = .04f;

                    for (const auto& joint : skeleton) {
                        const math::vec3 start = m * joint.position;
                        math::vec3 dir = math::vec3(0,1,0);
                        dir = m * math::TransformPointByQuaternion(joint.orientation, dir);
                        math::vec3 end = start + dir;
                        Renderer::SubmitLine(start, end, colors[c % colors.size()]);

                        //Renderer::SubmitLine(start + math::vec3(-s, 0, 0), start + math::vec3(s, 0, 0), colors[c % colors.size()]);
                        //Renderer::SubmitLine(start + math::vec3(0, -s, 0), start + math::vec3(0, s, 0), colors[c % colors.size()]);
                        //Renderer::SubmitLine(start + math::vec3(0, 0, -s), start + math::vec3(0, 0, s), colors[c % colors.size()]);
                        c++;
                    }

                    const auto& bindPose = mesh.Mesh->GetBindPose();
                    for (const auto& joint : bindPose) {
                        const math::vec3 start = m * joint.position;
                        math::vec3 dir = math::vec3(0, 1, 0);
                        dir = m * math::TransformPointByQuaternion(joint.orientation, dir);
                        math::vec3 end = start + dir;
                        Renderer::SubmitLine(start, end, math::vec4(.4,.4,.4,.8));
                    }
                }
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