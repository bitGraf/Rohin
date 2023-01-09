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
public:
    uint32 BaseVertex;
    uint32 BaseIndex;
    uint32 MaterialIndex;
    uint32 IndexCount;

    rh::laml::Mat4 Transform;
};

struct triangle_mesh {
    vertex_array_object VertexArray;
    //submesh* Submeshes;
};

void LoadMeshFromBuffer(triangle_mesh* Mesh, uint8* Buffer, uint32 BufferSize);

#endif