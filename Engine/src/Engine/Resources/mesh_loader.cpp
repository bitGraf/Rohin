#include "Resource_Manager.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Memory/Memory_Arena.h"

#include "Engine/Renderer/Renderer.h"

#include <cstring>

bool32 CheckTag(uint8* Buffer, char Token[], size_t NumChars) {
    for (int n = 0; n < NumChars; n++) {
        if (*Buffer++ != Token[n]) {
            return false;
        }
    }
    return true;
}

struct mesh_file_header {
    unsigned char Magic[4];
    uint32 FileSize;
    uint32 Flag;
    unsigned char Info[4];
    uint32 NumVerts;
    uint32 NumInds;
    uint16 NumSubmeshes;
    unsigned char VersionString[4];
    uint16 CommentLength;
};
struct mesh_file_submesh {
    char Tag[8];
    uint32 BaseIndex;
    uint32 MaterialIndex;
    uint32 IndexCount;
    real32 Transform[16];
};
struct mesh_file_joint {
    int32 ParentIndex;
    real32 DebugLength;
    real32 LocalMatrix[16];
    real32 InverseModelMatrix[16];
};

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

bool32 CheckHeader(mesh_file_header* Header, uint64 Size) {
    if (Header->FileSize != Size) return false;
    if (!CheckTag(Header->Magic, "MESH", 4)) return false;
    if (!CheckTag(Header->Info, "INFO", 4)) return false;

    if (Header->VersionString[0] != 'v') return false;
    uint8 VersionMajor = Header->VersionString[1];
    uint8 VersionMinor = Header->VersionString[2];
    uint8 VersionPatch = Header->VersionString[3];
    // check version number

    return true;
}

// buffer reading utils
#define AdvanceBufferArray(Buffer, Type, Count, End) (Type*)AdvanceBufferSize_(Buffer, (Count)*sizeof(Type), End)
#define AdvanceBuffer(Buffer, Type, End) (Type*)AdvanceBufferSize_(Buffer, sizeof(Type), End)
uint8* AdvanceBufferSize_(uint8** Buffer, uint32 Size, uint8* End) {
    Assert((*Buffer + Size) <= End);
    uint8* Result = *Buffer;
    *Buffer += Size;
    return Result;
}

mesh_file_result resource_load_mesh_file(const char* resource_file_name, 
                                         triangle_geometry** out_mesh,
                                         skeleton** out_skeleton,
                                         animation** out_animations, uint32* out_num_anims) {
    char full_path[256];
    platform_get_full_resource_path(full_path, 256, resource_file_name);

    RH_DEBUG("Fulle filename: [%s]", full_path);

    file_handle file = platform_read_entire_file(full_path);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");
        return mesh_file_result::error;
    }

    memory_arena* arena = resource_get_arena();

    // process the file buffer now to get the mesh data!
    uint8* End = file.data + file.num_bytes;
    mesh_file_header* Header = AdvanceBuffer(&file.data, mesh_file_header, End);

    if (!CheckHeader(Header, file.num_bytes)) {
        Assert(!"Incorrect header\n");
        return mesh_file_result::error;
    }
    AdvanceBufferSize_(&file.data, Header->CommentLength, End);

    // decode mesh flags
    bool32 HasSkeleton = Header->Flag & 0x01;

    uint8 /*Mesh->*/NumSubmeshes = (uint8)Header->NumSubmeshes;
    Assert(/*Mesh->*/NumSubmeshes == Header->NumSubmeshes); // 16->8 bits conversion
    ///*Mesh->Submeshes = */PushArray(arena, mesh_file_submesh, /*Mesh->*/NumSubmeshes);
    for (int SubmeshIndex = 0; SubmeshIndex < Header->NumSubmeshes; SubmeshIndex++) {
        mesh_file_submesh* BufferSubmesh = AdvanceBuffer(&file.data, mesh_file_submesh, End);
        //submesh* Submesh = &Mesh->Submeshes[SubmeshIndex];
        //Submesh->BaseVertex = 0;
        //Submesh->BaseIndex = BufferSubmesh->BaseIndex;
        //Submesh->MaterialIndex = BufferSubmesh->MaterialIndex; // NOTE: mesh file will always have 0 here...
        //Submesh->IndexCount = BufferSubmesh->IndexCount;
        //
        //Submesh->Transform = rh::laml::Mat4(BufferSubmesh->Transform);
    }

#if 0
    // TODO: Add proper material support to the file format
    material* Material = PushStruct(arena, material);
    renderer_create_texture(Material->diffuse_map, nullptr);
#endif

    if (HasSkeleton) {
        skeleton* Skeleton = PushStruct(arena, skeleton);

        AdvanceBufferSize_(&file.data, 4, End);
        uint16* NumJoints = AdvanceBuffer(&file.data, uint16, End);
        Skeleton->num_joints = (uint8)(*NumJoints);
        Assert(Skeleton->num_joints == (*NumJoints)); // 16->8 bits conversion
        Skeleton->joints = PushArray(arena, joint, Skeleton->num_joints);
        Skeleton->joints_debug = PushArray(arena, joint_debug, Skeleton->num_joints);

        for (uint16 JointIndex = 0; JointIndex < *NumJoints; JointIndex++) {
            uint16* JointNameLength = AdvanceBuffer(&file.data, uint16, End);
            char* JointName = (char*)AdvanceBufferSize_(&file.data, *JointNameLength, End);

            mesh_file_joint* BufferJoint = AdvanceBuffer(&file.data, mesh_file_joint, End);

            joint* Joint = &Skeleton->joints[JointIndex];
            Joint->parent_index = BufferJoint->ParentIndex;
            Joint->local_matrix = BufferJoint->LocalMatrix;
            Joint->inverse_model_matrix = BufferJoint->InverseModelMatrix;
            laml::identity(Joint->final_transform);

            joint_debug* JointDebug = &Skeleton->joints_debug[JointIndex];
            JointDebug->length = BufferJoint->DebugLength;
            JointDebug->name = PushArray(arena, char, *JointNameLength);
            // TODO: better string copy code lol
            for (uint16 nn = 0; nn < *JointNameLength; nn++) {
                JointDebug->name[nn] = JointName[nn];
            }
            JointDebug->model_matrix = laml::inverse(Joint->inverse_model_matrix); // slow!
        }

        // save to output
        if (out_skeleton) {
            *out_skeleton = Skeleton;
        }
    }

    // TODO: Do we need to cache the vertices/indices of the mesh? 
    // or just send them to the GPU and discard.
    // Easier to just discard, but we might need them.
    // Only use case I can think of is for the collision geometry, so maybe
    // we pass it to there first then discard it.
    AdvanceBufferArray(&file.data, char, 4, End); // DATA
    AdvanceBufferArray(&file.data, char, 4, End); // IDX\0

    uint32* Indices = AdvanceBufferArray(&file.data, uint32, Header->NumInds, End);

    AdvanceBufferArray(&file.data, char, 4, End); // VERT

    size_t VertexDataSize = HasSkeleton ? Header->NumVerts * sizeof(vertex_anim) : Header->NumVerts * sizeof(vertex_static) ;
    void* VertexData = AdvanceBufferSize_(&file.data, (uint32)VertexDataSize, End);

    if (HasSkeleton) {
        AdvanceBufferArray(&file.data, char, 4, End); // ANIM

        uint16* NumAnimsInCatalog = AdvanceBuffer(&file.data, uint16, End);
        uint32 num_animations = (uint32)(*NumAnimsInCatalog);
        Assert(num_animations == *NumAnimsInCatalog);
        animation* Animations = PushArray(arena, animation, num_animations);

        for (uint16 Animindex = 0; Animindex < *NumAnimsInCatalog; Animindex++) {
            uint16* AnimNameLen = AdvanceBuffer(&file.data, uint16, End);
            char* AnimName = AdvanceBufferArray(&file.data, char, *AnimNameLen, End);

            // No actual animation data is stored here, just the names to look up later...
            //animation* Animation = &Mesh->Animations[Animindex];

            animation* Animation = &Animations[Animindex];
            Animation->name = PushArray(arena, char, *AnimNameLen);
            // TODO: better string copy code lol
            for (uint16 nn = 0; nn < *AnimNameLen; nn++) {
                Animation->name[nn] = AnimName[nn];
            }
        }

        // save to output
        if (out_animations && out_num_anims) {
            *out_num_anims = num_animations;
            *out_animations = Animations;
        }
    }

    uint8* EndTag = AdvanceBufferArray(&file.data, uint8, 4, End); // VERT
    if (!CheckTag(EndTag, "END", 4)) {
        Assert(!"Did not end up at the correct place in the file ;~;\n");
    }
    Assert(file.data == End);

    // Pass onto the renderer to create
    triangle_geometry* geom = PushStruct(arena, triangle_geometry);
    if (HasSkeleton) {
        renderer_create_mesh(geom, Header->NumVerts, VertexData, Header->NumInds, Indices, static_mesh_attribute_list);
    } else {
        renderer_create_mesh(geom, Header->NumVerts, VertexData, Header->NumInds, Indices, dynamic_mesh_attribute_list);
    }

    // save to output
    if (out_mesh) {
        *out_mesh = geom;
    }

    platform_free_file_data(&file);

    return HasSkeleton ? mesh_file_result::is_animated : mesh_file_result::is_static;
}