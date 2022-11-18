#pragma once

#include <vector>

#include "Engine/Core/Base.hpp"
#include "Engine/Core/Timing.hpp"
#include "Engine/Core/GameMath.hpp"

#include "Engine/Renderer/VertexArray.hpp"
#include "Engine/Renderer/Buffer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Material.hpp"

namespace Engine {

    class Submesh
    {
    public:
        //u32 BaseVertex;
        u32 BaseIndex;
        u32 MaterialIndex;
        u32 IndexCount;

        math::mat4 Transform;
    };

    struct Joint {
        math::mat4 invTransform; // in object-space
        s32 parent;
        std::string name;

        // Bind-pose transform
        math::vec3 position;
        math::quat orientation;

        // TODO: REMOVE THIS
        math::mat4 Transform;
    };

    class Mesh
    {
    public:
        // NEW constructor
        Mesh(const std::string& filename);

        // load MESH_File
        Mesh(const std::string& filename, int);
        // load NBT .mesh file
        Mesh(const std::string& filename, float, float);
        ~Mesh();

        bool Loaded() { return m_loaded; }

        void OnUpdate(double dt);

        std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
        const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

        Ref<Shader> GetMeshShader() const { return m_MeshShader; }
        Ref<Material> GetBaseMaterial() const { return m_BaseMaterial; }
        const std::vector<Ref<MaterialInstance>>& GetMaterials() const { return m_Materials; }
        Ref<MaterialInstance> GetMaterial(u32 index) { ENGINE_LOG_ASSERT(index <= m_Materials.size(), "Not that many materials!"); return m_Materials[index]; }

        const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
        // ANIM_HOOK const std::vector<md5::Joint>&  GetBindPose() const { return m_BindPose; }
    private:

    private:
        // Hardware buffer of verts
        Ref<VertexArray> m_VertexArray;

        // submesh info
        std::vector<Submesh> m_Submeshes;

        // Materials
        Ref<Shader> m_MeshShader;
        Ref<Material> m_BaseMaterial;
        std::vector<Engine::Ref<Engine::Texture2D>> m_Textures;
        std::vector<Engine::Ref<Engine::MaterialInstance>> m_Materials;
        // ANIM_HOOK std::vector<md5::Joint> m_BindPose; // TODO: only relevant with animation

        bool m_loaded = false;

        // Animation stuff
        bool m_isAnimated = false;

        friend class Renderer;
    };
}
