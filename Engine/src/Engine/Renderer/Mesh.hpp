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

    // Use this moving forward
    struct Vertex
    {
        math::vec3 Position;
        math::vec3 Normal;
        math::vec3 Tangent;
        math::vec3 Bitangent;
        math::vec2 Texcoord;
    };

    // TODO: normalize this to be like a static_mesh vertex
    struct Vertex_Anim
    {
        math::vec3 Position;
        math::vec3 Normal;
        math::vec3 Tangent;
        math::vec3 Bitangent;
        math::vec2 Texcoord;

        s32 BoneIndices[4];
        math::vec4 BoneWeights;
    };

    struct Triangle
    {
        u32 V1, V2, V3;
    };

    struct SkeleJoint {
        math::mat4 inverseBindPose;
        math::mat4 finalTransform;
    };

    struct BoneFrame {
        math::vec3 position;
        math::quat rotation;
        math::vec3 scale;
    };

    struct AnimFrame {
        std::vector<BoneFrame> bones;
    };

    struct Animation {
        std::string name;
        u16 num_frames;
        u16 num_channels;
        f64 frames_per_second;

        std::vector<AnimFrame> frames;
    };

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
        // MESH_File
        Mesh(const std::string& filename);
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

        void SetCurrentAnimation(const std::string& anim_name);
    private:
        void populateAnimationData(const std::string& filename);
        void UpdateSkeleton(int frame1, int frame2, double interp);

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
        std::vector<SkeleJoint> m_Skeleton;
        std::unordered_map<std::string, Animation> m_Animations;
        Animation* m_currentAnim = nullptr;
        bool m_hasAnimations = false;
        f64 m_animTime = 0.0;

        friend class Renderer;
    };
}
