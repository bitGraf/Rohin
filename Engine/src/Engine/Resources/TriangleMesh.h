#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "Engine/Defines.h"
#include <laml/laml.hpp>

struct vertex_static {
    laml::Vec3 Position;
    laml::Vec3 Normal;
    laml::Vec3 Tangent;
    laml::Vec3 Bitangent;
    laml::Vec2 Texcoord;
};

struct vertex_anim {
    laml::Vec3 Position;
    laml::Vec3 Normal;
    laml::Vec3 Tangent;
    laml::Vec3 Bitangent;
    laml::Vec2 Texcoord;

    int32 BoneIndices[4];
    laml::Vec4 BoneWeights;
};

struct submesh {
    uint32 BaseVertex;
    uint32 BaseIndex;
    uint32 MaterialIndex;
    uint32 IndexCount;

    laml::Mat4 Transform;
};

struct joint_debug {
    real32 Length;
    char* Name;
    laml::Mat4 ModelMatrix;
};

struct joint {
    static const int32 NullIndex = -1;

    int32 ParentIndex;
    laml::Mat4 LocalMatrix;
    laml::Mat4 InverseModelMatrix;
    laml::Mat4 FinalTransform;
};

struct animation_debug {
    char* Name;
};

struct animation {
    // TODO:
};

struct material {
    //texture_2D Diffuse;
    //texture_2D Normal;
};

struct triangle_mesh {
    //vertex_array_object VertexArray;

    // no more than 255 of each of these... seems reasonable
    // if NumBones is set to 0, mesh has no animation data associated.
    uint8 NumSubmeshes;
    uint8 NumMaterials;
    uint8 NumJoints;
    uint8 NumAnimations;

    submesh* Submeshes;
    material* Materials;
    joint* Joints;
    animation* Animations;

#if 1
    joint_debug* JointsDebug;
    animation_debug* AnimationsDebug;
#endif
};

#endif