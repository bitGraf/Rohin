#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Core/GameMath.hpp"

namespace Engine {

    namespace md5 {

        ///////////////////////////////////////
        // MD5Mesh ////////////////////////////
        ///////////////////////////////////////

        struct Joint {
            math::mat4 invTransform; // in object-space
            s32 parent;
            std::string name;

            // Bind-pose transform
            math::vec3 position;
            math::quat orientation;
        };

        struct Vert {
            math::vec3 position;
            math::vec3 normal;
            math::vec4 tangent;
            math::vec2 uv;
            int boneIndices[4];
            math::vec4 boneWeights;
        };

        struct Tri {
            int vertIndex[3];
        };

        struct Mesh {
            std::string Shader; // actually a material name

            std::vector<Vert> Verts;
            std::vector<Tri> Tris;
        };

        struct Model {
            int numJoints;
            int numMeshes;

            std::vector<Joint> Joints;
            std::vector<Mesh> Meshes;
        };

        bool LoadMD5MeshFile(const std::string& filename, Model* model);


        ///////////////////////////////////////
        // MD5Material ////////////////////////
        ///////////////////////////////////////

        struct Material {
            std::string name;

            std::string diffusemap;
            std::string normalmap;
            std::string specularmap;
        };

        bool LoadMD5MaterialDefinitionFile(const std::string& filename, std::unordered_map<std::string, Material>& map);


        ///////////////////////////////////////
        // MD5Anim ////////////////////////////
        ///////////////////////////////////////

        struct JointInfo {
            std::string name;
            int parentID;
            int flags;
            int startIndex;
        };

        struct Bound {
            math::vec3 min;
            math::vec3 max;
        };

        struct BaseFrame {
            math::vec3 position;
            math::quat orientation;
        };

        struct FrameData {
            std::vector<float> frameData;
        };

        struct SkeletonJoint {
            SkeletonJoint() {}
            SkeletonJoint(const BaseFrame& copy) : position(copy.position), orientation(copy.orientation) {}

            math::vec3 position;
            math::quat orientation;

            math::mat4 transform;
        };

        struct FrameSkeleton {
            std::vector<SkeletonJoint> Joints;
        };

        struct Animation {
            std::vector<JointInfo>       JointInfos;
            std::vector<Bound>           Bounds;
            std::vector<FrameSkeleton>   Skeletons;    // All the skeletons for all the frames

            FrameSkeleton       AnimatedSkeleton;

            int numFrames;
            int numJoints;
            int FrameRate;
            int numAnimComponents;

            float animDuration;
            float frameDuration;
            float animTime;
        };

        bool LoadMD5AnimFile(const std::string& filename, Animation* anim);
        void UpdateMD5Animation(Animation* anim, float deltaTime);
        void BuildFrameSkeleton(
            std::vector<FrameSkeleton>& skele,
            const std::vector<JointInfo>& jointInfo,
            const std::vector<BaseFrame>& baseFrames,
            const FrameData& frame);
    }
}