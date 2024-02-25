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
};
struct mesh_file_vertex {
    laml::Vec3 Position;
    laml::Vec3 Normal;
    laml::Vec3 Tangent;
    laml::Vec3 Bitangent;
    laml::Vec2 UV;
};
struct mesh_file_primitive {
    mesh_file_primitive_header Header;

    uint32 *Indices;
    mesh_file_vertex *Vertices;
};

struct mesh_file {
    mesh_file_header Header;

    mesh_file_material *Materials;
    mesh_file_primitive *Primitives;
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