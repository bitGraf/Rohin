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

    struct Vertex
    {
        math::vec3 Position;
        math::vec3 Normal;
        math::vec3 Tangent;
        math::vec3 Binormal;
        math::vec2 Texcoord;
    };

    struct Vertex_Anim
    {
        math::vec3 Position;
        math::vec3 Normal;
        math::vec3 Tangent;
        math::vec3 Binormal;
        math::vec2 Texcoord;
        s32 BoneIndices[4];
        math::vec4 BoneWeights;
    };

    struct Triangle
    {
        uint32_t V1, V2, V3;
    };

    static_assert(sizeof(Triangle) == 3 * sizeof(uint32_t));

    class Submesh
    {
    public:
        //u32 BaseVertex;
        u32 BaseIndex;
        u32 MaterialIndex;
        u32 IndexCount;

        math::mat4 Transform;

        std::string SubmeshName;
    };

    class Mesh
    {
    public:
        // load NBT .mesh file
        Mesh(const std::string& filename);
        Mesh(const md5::Model& model);
        ~Mesh();

        //bool LoadFromFile();
        //bool LoadFromMD5();
        bool Loaded() { return m_loaded; }

        void OnUpdate(double ts);
        void DumpVertexBuffer();

        std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
        const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

        Ref<Shader> GetMeshShader() { return m_MeshShader; }
        Ref<Material> GetBaseMaterial() { return m_BaseMaterial; }
        std::vector<Ref<MaterialInstance>> GetMaterials() { return m_Materials; }
        Ref<MaterialInstance> GetMaterial(u32 index) { ENGINE_LOG_ASSERT(index <= m_Materials.size(), "Not that many materials!"); return m_Materials[index]; }
        //const std::vector<Engine::Ref<Engine::Texture2D>>& GetTextures() const { return m_Textures; }

        //const std::string& GetName() const { return m_Name; }
        //const std::string& GetFilePath() const { return m_FilePath; }
        //const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
        //const std::vector<Triangle>& GetTris() const { return m_Tris; }

        const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
        const std::vector<md5::Joint>&  GetBindPose() const { return m_BindPose; }
    private:

    private:
        Ref<VertexArray> m_VertexArray;

        //std::vector<Vertex> m_Vertices;
        //std::vector<Triangle> m_Tris;
        std::vector<Submesh> m_Submeshes;

        // Materials
        Ref<Shader> m_MeshShader;
        Ref<Material> m_BaseMaterial;
        std::vector<Engine::Ref<Engine::Texture2D>> m_Textures;
        std::vector<Engine::Ref<Engine::MaterialInstance>> m_Materials;
        std::vector<md5::Joint> m_BindPose;

        bool m_loaded = false;

        friend class Renderer;
    };
}
