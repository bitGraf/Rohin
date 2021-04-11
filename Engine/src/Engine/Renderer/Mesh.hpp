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
#include "Engine/Resources/MD5MeshLoader.hpp"

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

    class Mesh
    {
    public:
        // load NBT .mesh file
        Mesh(const std::string& filename);
        // load from .MD5Mesh file
        Mesh(const md5::Model& model);
        ~Mesh();

        bool Loaded() { return m_loaded; }

        std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
        const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

        Ref<Shader> GetMeshShader() { return m_MeshShader; }
        Ref<Material> GetBaseMaterial() { return m_BaseMaterial; }
        std::vector<Ref<MaterialInstance>> GetMaterials() { return m_Materials; }
        Ref<MaterialInstance> GetMaterial(u32 index) { ENGINE_LOG_ASSERT(index <= m_Materials.size(), "Not that many materials!"); return m_Materials[index]; }

        const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
        const std::vector<md5::Joint>&  GetBindPose() const { return m_BindPose; }
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
        std::vector<md5::Joint> m_BindPose; // TODO: only relevant with animation

        bool m_loaded = false;

        friend class Renderer;
    };
}
