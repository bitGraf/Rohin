#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include <laml/laml.hpp>

struct vertex_static {
    rh::laml::Vec3 Position;
    rh::laml::Vec3 Normal;
    rh::laml::Vec3 Tangent;
    rh::laml::Vec3 Bitangent;
    rh::laml::Vec2 Texcoord;
};

struct vertex_anim {
    rh::laml::Vec3 Position;
    rh::laml::Vec3 Normal;
    rh::laml::Vec3 Tangent;
    rh::laml::Vec3 Bitangent;
    rh::laml::Vec2 Texcoord;

    rh::s32 BoneIndices[4];
    rh::laml::Vec4 BoneWeights;
};

struct submesh {
    uint32 BaseVertex;
    uint32 BaseIndex;
    uint32 MaterialIndex;
    uint32 IndexCount;

    rh::laml::Mat4 Transform;
};

struct joint_debug {
    real32 Length;
    char* Name;
    rh::laml::Mat4 ModelMatrix;
};

struct joint {
    static const int32 NullIndex = -1;

    int32 ParentIndex;
    rh::laml::Mat4 LocalMatrix;
    rh::laml::Mat4 InverseModelMatrix;
    rh::laml::Mat4 FinalTransform;
};

struct animation_debug {
    char* Name;
};

struct animation {
    // TODO:
};

struct material {
    texture_2D Diffuse;
    //texture_2D Normal;
};

struct triangle_mesh {
    vertex_array_object VertexArray;

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

void LoadMeshFromBuffer(memory_arena* Arena, triangle_mesh* Mesh, uint8* Buffer, uint32 BufferSize);

#endif