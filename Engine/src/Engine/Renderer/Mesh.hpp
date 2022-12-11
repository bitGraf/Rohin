#pragma once

#include <vector>

#include "Engine/Core/Base.hpp"
#include "Engine/Core/Timing.hpp"

#include "Engine/Renderer/VertexArray.hpp"
#include "Engine/Renderer/Buffer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Material.hpp"

namespace rh {

    // Use this moving forward
    struct Vertex
    {
        laml::Vec3 Position;
        laml::Vec3 Normal;
        laml::Vec3 Tangent;
        laml::Vec3 Bitangent;
        laml::Vec2 Texcoord;
    };

    // TODO: normalize this to be like a static_mesh vertex
    struct Vertex_Anim
    {
        laml::Vec3 Position;
        laml::Vec3 Normal;
        laml::Vec3 Tangent;
        laml::Vec3 Bitangent;
        laml::Vec2 Texcoord;

        s32 BoneIndices[4];
        laml::Vec4 BoneWeights;
    };

    struct Triangle
    {
        u32 V1, V2, V3;
    };

    struct SkeleJoint {
        s32 parent_idx;
        laml::Mat4 local_matrix;
        laml::Mat4 inverse_model_matrix;
        std::string bone_name;

        laml::Mat4 finalTransform;

        laml::Mat4 model_matrix;
    };

    struct Skeleton {
        static const s32 NullIndex = -1;

        u32 num_bones;

        std::vector<SkeleJoint> bones;
    };

    const u32 CONSTANT_TRANSLATION = 1;
    const u32 CONSTANT_ROTATION = 2;
    const u32 CONSTANT_SCALE = 4;

    struct AnimNode {
        u32 node_flag;
        std::vector<laml::Vec3> translations;
        std::vector<laml::Quat> rotations;
        std::vector<laml::Vec3> scales;
    };

    struct Animation {
        f32 duration;
        f32 frame_rate;
        u32 num_nodes;
        u32 num_samples;

        std::string name;
        std::vector<AnimNode> nodes;
    };

    class Submesh
    {
    public:
        //u32 BaseVertex;
        u32 BaseIndex;
        u32 MaterialIndex;
        u32 IndexCount;

        laml::Mat4 Transform;
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

        void OnUpdate(float dt);

        std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
        const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

        Ref<Shader> GetMeshShader() const { return m_MeshShader; }
        Ref<Material> GetBaseMaterial() const { return m_BaseMaterial; }
        const std::vector<Ref<MaterialInstance>>& GetMaterials() const { return m_Materials; }
        Ref<MaterialInstance> GetMaterial(u32 index) { ENGINE_LOG_ASSERT(index <= m_Materials.size(), "Not that many materials!"); return m_Materials[index]; }

        const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
        // ANIM_HOOK const std::vector<md5::Joint>&  GetBindPose() const { return m_BindPose; }

        void SetCurrentAnimation(const std::string& anim_name);
        inline bool HasAnimations() const { return m_hasAnimations; }
        inline const Skeleton& GetSkeleton() const { return m_Skeleton; }
    private:
        void populateAnimationData(const std::string& filename);
        //void SampleAnimation(float frame_time);
        void UpdateSkeleton(u32 frame1, u32 frame2, f32 interp);

    private:
        // Hardware buffer of verts
        Ref<VertexArray> m_VertexArray;

        // submesh info
        std::vector<Submesh> m_Submeshes;

        // Materials
        Ref<Shader> m_MeshShader;
        Ref<Material> m_BaseMaterial;
        std::vector<rh::Ref<rh::Texture2D>> m_Textures;
        std::vector<rh::Ref<rh::MaterialInstance>> m_Materials;
        // ANIM_HOOK std::vector<md5::Joint> m_BindPose; // TODO: only relevant with animation

        bool m_loaded = false;

        // Animation stuff
        Skeleton m_Skeleton;
        std::unordered_map<std::string, Animation> m_Animations;
        Animation* m_currentAnim = nullptr;
        bool m_hasAnimations = false;
        f64 m_animTime = 0.0;

        friend class Renderer;
    };
}
