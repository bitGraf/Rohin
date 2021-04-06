#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Core/GameMath.hpp"

namespace Engine {

    namespace md5 {

        ///////////////////////////////////////
        // MD5Mesh ////////////////////////////
        ///////////////////////////////////////

        struct Joint {
            std::string name;
            s32 parent;
            math::vec3 position;
            math::quat orientation;
        };

        struct Vert {
            // from file
            math::vec2 uv;
            int startWeight;
            int countWeight;

            // calculated
            math::vec3 position;
            math::vec3 normal;
        };

        struct Tri {
            int vertIndex[3];
        };

        struct Weight {
            int weightIndex;
            int joint;
            float bias; // [0.0 1.0]
            math::vec3 pos;
        };

        struct Mesh {
            std::string Shader;

            std::vector<Vert> Verts;
            std::vector<Tri> Tris;
            std::vector<Weight> Weights;
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
            int frameID;
            std::vector<float> frameData;
        };

        struct SkeletonJoint {
            SkeletonJoint() : parentID(-1) {}
            SkeletonJoint(const BaseFrame& copy) : position(copy.position), orientation(copy.orientation) {}

            int parentID;
            math::vec3 position;
            math::quat orientation;
        };

        struct FrameSkeleton {
            std::vector<SkeletonJoint> Joints;
        };

        struct Animation {
            std::vector<JointInfo>       JointInfos;
            std::vector<Bound>           Bounds;
            std::vector<BaseFrame>       BaseFrames;
            std::vector<FrameData>       Frames;
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