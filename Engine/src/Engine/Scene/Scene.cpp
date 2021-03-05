#include <enpch.hpp>
#include "Scene.hpp"

#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/GameObject/Components.hpp"

/*mat4 transform = (
    mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(position, 1)) *
    mat4(createYawPitchRollMatrix(0, 0, 0)));*/

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
        // Update
        auto& scriptComponents = m_Registry.view<NativeScriptComponent>();
        for (auto& script : scriptComponents) {
            if (script.Script) {
                ENGINE_LOG_ASSERT(script.Script, "Native script not instantiated");

                script.Script->OnUpdate(dt);
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

        // Render
        if (mainCamera) {
            Renderer::BeginScene(*mainCamera, *mainTransform);

            // messy group
            // TODO: figure out how to get the group function to work
            for (auto n : m_Registry.GetRegList()) {
                // check if entity n has both transform and mesh
                if (m_Registry.has<TransformComponent>(n) && m_Registry.has<MeshRendererComponent>(n)) {
                    // good to go
                    auto& mesh = m_Registry.get<MeshRendererComponent>(n);
                    auto& trans = m_Registry.get<TransformComponent>(n);
                    auto& tag = m_Registry.get<TagComponent>(n);

                    if (mesh.Mesh)
                        Renderer::SubmitMesh(mesh.Mesh, trans.Transform);
                }
            }

            Renderer::EndScene();
        }
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