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
        laml::Mat4 inverseBindPose;
        laml::Mat4 finalTransform;
        std::string bone_name;
        s32 parent_idx;
    };

    struct Skeleton {
        std::vector<SkeleJoint> m_bones;
    };

    struct BoneFrame {
        laml::Vec3 position;
        laml::Quat rotation;
        laml::Vec3 scale;
    };

    struct AnimFrame {
        std::vector<BoneFrame> bones;
    };

    // A single keyed (by frame time and track number) value for an animation
    template<typename T>
    struct AnimationKey
    {
        T Value;
        f32 FrameTime;       // 0.0f = beginning of animation clip, 1.0f = end of animation clip 
        u32 Track;

        //AnimationKey(const f32 frameTime, const u32 track, const T& value)
        //    : FrameTime(frameTime)
        //    , Track(track)
        //    , Value(value)
        //{}
    };
    using TranslationKey = AnimationKey<laml::Vec3>;
    using RotationKey = AnimationKey<laml::Quat>;
    using ScaleKey = AnimationKey<laml::Vec3>;

    struct Animation {
        std::string name;
        u16 num_channels;
        u16 num_translate_keys;
        u16 num_rotate_keys;
        u16 num_scale_keys;
        f32 duration;

        std::vector<TranslationKey> translate_keys;
        std::vector<RotationKey> rotate_keys;
        std::vector<ScaleKey> scale_keys;
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

    template<typename T>
    struct SampleCache {
        std::vector<T> m_values;
        std::vector<float> m_frame_times;

        const Animation* m_anim;

        float m_prev_sample_time;
        u32 m_cursor;

        void resize(u32 num_channels) {
            m_values.resize(num_channels * 2, T());
        }
        void reset(const Animation* anim, const std::vector<T>& values) {
            for (u32 i = 0, N = static_cast<u32>(values.size()); i < N; ++i) {
                m_values[NextIndex(i)] = values[i];
                m_frame_times[NextIndex(i)] = 0.0f;
            }
            m_anim = anim;
            m_cursor = 0;
        }
        void step(float sample_time, const std::vector<AnimationKey<T>>& keys) {
            if ((m_cursor == static_cast<u32>(keys.size())) || (sample_time < m_prev_sample_time)) {
                Loop();
            }
            auto track = keys[m_cursor].Track;
            while (m_frame_times[NextIndex(track)] <= sample_time) {
                m_values[CurrentIndex(track)] = m_values[NextIndex(track)];
                m_values[NextIndex(track)] = keys[m_cursor].Value;
                m_frame_times[CurrentIndex(track)] = m_frame_times[NextIndex(track)];
                m_frame_times[NextIndex(track)] = keys[m_cursor].FrameTime;

                if (++m_cursor == static_cast<u32>(keys.size()))
                {
                    break;
                }
                track = keys[m_cursor].Track;
            }
            m_prev_sample_time = sample_time;
        }

        void Loop() {
            m_cursor = 0;
            for (u32 track = 0, N = static_cast<u32>(m_values.size() / 2); track < N; ++track)
            {
                m_frame_times[NextIndex(track)] = 0.0;
            }
            m_prev_sample_time = 0.0f;
        }

        void Interpolate(const f32 sampleTime, std::vector<T>& result, const std::function<T(const T&, const T&, const float)>& interpolater)
        {
            for (uint32_t i = 0, N = static_cast<uint32_t>(m_values.size()); i < N; i += 2)
            {
                const f32 t = (sampleTime - m_frame_times[i]) / (m_frame_times[i + 1] - m_frame_times[i]);

                result[i / 2] = interpolater(m_values[i], m_values[i + 1], t);
            }
        }

        static u32 CurrentIndex(const u32 i) { return 2 * i; }
        static u32 NextIndex(const u32 i) { return 2 * i + 1; }
    };

    struct AnimCache {
        SampleCache<laml::Vec3> translate_cache;
        SampleCache<laml::Quat> rotate_cache;
        SampleCache<laml::Vec3> scale_cache;

        std::vector<laml::Vec3> m_LocalTranslations;
        std::vector<laml::Quat> m_LocalRotations;
        std::vector<laml::Vec3> m_LocalScales;

        void resize(uint32_t numBones)
        {
            translate_cache.resize(numBones);
            rotate_cache.resize(numBones);
            scale_cache.resize(numBones);
        }
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
        void SampleAnimation(float frame_time);
        void UpdateSkeleton(int frame1, int frame2, double interp);

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
        AnimCache m_animCache;

        friend class Renderer;
    };
}
