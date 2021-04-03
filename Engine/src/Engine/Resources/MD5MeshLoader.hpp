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
    }
}