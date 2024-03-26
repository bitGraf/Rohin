#pragma once

#include "Engine/Resources/Resource_Types.h"

struct mesh_file_header {
    unsigned char Magic[4];
    uint32 FileSize;
    uint32 Version;
    uint32 Flag;
    uint64 Timestamp;
    uint16 NumPrims;
    uint16 PADDING[3];
};
struct mesh_file_material_header {
    unsigned char Magic[4];
    uint32 Flag;
    real32 DiffuseFactor[3];
    real32 NormalScale;
    real32 AmbientStrength;
    real32 MetallicFactor;
    real32 RoughnessFactor;
    real32 EmissiveFactor[3];
};
struct mesh_file_string {
    uint8 len;
    char str[FILE_MAX_STRING_LENGTH];
};
struct mesh_file_material {
    mesh_file_material_header Header;

    mesh_file_string Name;

    mesh_file_string DiffuseTexture;
    mesh_file_string NormalTexture;
    mesh_file_string AMRTexture;
    mesh_file_string EmissiveTexture;
};
struct mesh_file_primitive_header {
    unsigned char Magic[4];
    uint32 NumVerts;
    uint32 NumInds;
    uint32 MatIdx;
    uint32 PrimType;
};
struct mesh_file_vertex_static {
    laml::Vec3 Position;
    laml::Vec3 Normal;
    laml::Vec3 Tangent;
    laml::Vec3 Bitangent;
    laml::Vec2 UV;
};
struct mesh_file_vertex_skinned {
    laml::Vec3 Position;
    laml::Vec3 Normal;
    laml::Vec3 Tangent;
    laml::Vec3 Bitangent;
    laml::Vec2 UV;
    laml::Vector<int32, 4>  BoneIndices;
    laml::Vector<real32, 4> BoneWeights;
};
struct mesh_file_vertex_line {
    laml::Vec3 Position;
};
constexpr uint64 skinned_vert_size = sizeof(mesh_file_vertex_skinned);
constexpr uint64 static_vert_size  = sizeof(mesh_file_vertex_static);
constexpr uint64 line_vert_size    = sizeof(mesh_file_vertex_line);

struct mesh_file_skeleton_header {
    unsigned char Magic[4];
    uint32 NumBones;
};
struct mesh_file_primitive {
    mesh_file_primitive_header Header;

    uint32 *Indices;
    uint64 VertexAttribsSize;
    void* Vertices;
};
struct mesh_file_bone {
    uint32 bone_idx;
    int32 parent_idx;
    real32 debug_length;
    laml::Mat4 local_matrix;
    laml::Mat4 inv_model_matrix;

    mesh_file_string name;
};
struct mesh_file_skeleton {
    mesh_file_skeleton_header Header;

    mesh_file_bone* Bones;
};

struct mesh_file {
    mesh_file_header Header;

    mesh_file_material *Materials;
    mesh_file_primitive *Primitives;
    mesh_file_skeleton Skeleton;
};

mesh_file_result parse_mesh_file(const char* resource_file_name, mesh_file **file_data, memory_arena * arena);

#if 0
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
#endif