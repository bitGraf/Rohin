#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Core/GameMath.hpp"
#include "Engine/Scene/SceneCamera.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/GameObject/ScriptableBase.hpp"
#include <ostream>

namespace Engine {

    struct TagComponent {
        std::string Name;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& name) : Name(name) {}

        DEBUG_OSTR_IMPL(TagComponent)
    };

    struct MeshRendererComponent {
        Ref<Engine::Mesh> Mesh;

        MeshRendererComponent() = default;
        MeshRendererComponent(const MeshRendererComponent&) = default;
        MeshRendererComponent(const Ref<Engine::Mesh>& mesh) : Mesh(mesh) {}

        DEBUG_OSTR_IMPL(MeshRendererComponent)
    };
    struct TransformComponent {
        math::mat4 Transform;

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const math::mat4& transform) : Transform(transform) {}

        DEBUG_OSTR_IMPL(TransformComponent)
    };

    struct CameraComponent {
        SceneCamera camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;

        DEBUG_OSTR_IMPL(CameraComponent)
    };

    

    struct NativeScriptComponent {
        ScriptableBase* Script = nullptr;
        GameObject_type GameObjectID;
        //bool initialized = false;

        NativeScriptComponent() = default;
        NativeScriptComponent(const NativeScriptComponent&) = default;
        //NativeScriptComponent(ScriptableBase* base, GameObject go) : Script(base), GameObjectID(go.GetHandle()) {}

        ScriptableBase*(*InstantiateScript)();
        void (*DestroyScript)(NativeScriptComponent*);

        template<typename InstanceType>
        void Bind(GameObject go) {
            InstantiateScript = []() {
                return static_cast<ScriptableBase*>(new InstanceType()); 
            };
            DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Script; nsc->Script = nullptr; };
            GameObjectID = go.GetHandle();
        }

        template<typename InstanceType>
        InstanceType* GetScript() {
            if (Script)
                return static_cast<InstanceType*>(Script);
            return nullptr;
        }

        DEBUG_OSTR_IMPL(NativeScriptComponent)
};

#if 0
#define SIMPLE_COMP_CREATE(name) struct name {int val; name() = default; name(const name&) = default; name(int v) : val(v) {} DEBUG_OSTR_IMPL(name)};

    SIMPLE_COMP_CREATE(C1)
    SIMPLE_COMP_CREATE(C2)
    SIMPLE_COMP_CREATE(C3)
#endif

}