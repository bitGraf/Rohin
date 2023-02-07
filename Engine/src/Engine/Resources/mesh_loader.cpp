#include "Resource_Manager.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/String.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Memory/MemoryUtils.h"

#include "Engine/Renderer/Renderer.h"

#include "Engine/Collision/Collision.h"

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

mesh_file_result resource_load_mesh_file(const char* resource_file_name, 
                                         triangle_geometry* out_mesh,
                                         skeleton* out_skeleton,
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
            memory_copy(out_skeleton, Skeleton, sizeof(skeleton));
        }
        // can probably get rid of the memory allocated on the arena after
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
            memory_copy(out_num_anims, &num_animations, sizeof(uint32));
            memory_copy(out_animations, &Animations, sizeof(animation)*num_animations);
        }
    } else {
        if (out_num_anims) {
            *out_num_anims = 0;
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
        renderer_create_mesh(geom, Header->NumVerts, VertexData, Header->NumInds, Indices, dynamic_mesh_attribute_list);
    } else {
        renderer_create_mesh(geom, Header->NumVerts, VertexData, Header->NumInds, Indices, static_mesh_attribute_list);
    }

    // save to output
    if (out_mesh) {
        memory_copy(out_mesh, geom, sizeof(triangle_geometry));
    }

    platform_free_file_data(&file);

    return HasSkeleton ? mesh_file_result::is_animated : mesh_file_result::is_static;
}


bool32 resource_load_anim_file(animation* anim) {
    return false;
}





mesh_file_result resource_load_mesh_file_for_level(const char* resource_file_name,
                                                   triangle_geometry* out_geom,
                                                   collision_grid* grid) {

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
        // can probably get rid of the memory allocated on the arena after
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

    size_t individual_vertex_size = HasSkeleton ? sizeof(vertex_anim) : sizeof(vertex_static);
    size_t VertexDataSize = Header->NumVerts * individual_vertex_size;
    void* VertexData = AdvanceBufferSize_(&file.data, (uint32)VertexDataSize, End);

    // push triangles to collision grid
    memory_index arena_save = arena->Used;
    uint32 num_tris = Header->NumInds / 3;
    collision_triangle* triangles = PushArray(arena, collision_triangle, num_tris);
    if (HasSkeleton) {
        for (uint32 n = 0; n < num_tris; n++) {
            uint32 I0 = Indices[n*3 + 0];
            uint32 I1 = Indices[n*3 + 1];
            uint32 I2 = Indices[n*3 + 2];

            vertex_anim* verts = (vertex_anim*)VertexData;
            
            triangles[n].v1 = verts[I0].Position;
            triangles[n].v2 = verts[I1].Position;
            triangles[n].v3 = verts[I2].Position;

            collision_grid_add_triangle(arena, grid, triangles[n], true);
        }

        for (uint32 n = 0; n < num_tris; n++) {
            collision_grid_add_triangle(arena, grid, triangles[n], false);
        }
    } else {
        for (uint32 n = 0; n < num_tris; n++) {
            uint32 I0 = Indices[n*3 + 0];
            uint32 I1 = Indices[n*3 + 1];
            uint32 I2 = Indices[n*3 + 2];

            vertex_static* verts = (vertex_static*)VertexData;
            
            triangles[n].v1 = verts[I0].Position;
            triangles[n].v2 = verts[I1].Position;
            triangles[n].v3 = verts[I2].Position;

            collision_grid_add_triangle(arena, grid, triangles[n], true);
        }

        for (uint32 n = 0; n < num_tris; n++) {
            collision_grid_add_triangle(arena, grid, triangles[n], false);
        }
    }
    //arena->Used = arena_save;

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
    }

    uint8* EndTag = AdvanceBufferArray(&file.data, uint8, 4, End); // VERT
    if (!CheckTag(EndTag, "END", 4)) {
        Assert(!"Did not end up at the correct place in the file ;~;\n");
    }
    Assert(file.data == End);

    // Pass onto the renderer to create
    triangle_geometry* geom = PushStruct(arena, triangle_geometry);
    if (HasSkeleton) {
        renderer_create_mesh(geom, Header->NumVerts, VertexData, Header->NumInds, Indices, dynamic_mesh_attribute_list);
    } else {
        renderer_create_mesh(geom, Header->NumVerts, VertexData, Header->NumInds, Indices, static_mesh_attribute_list);
    }

    // save to output
    if (out_geom) {
        memory_copy(out_geom, geom, sizeof(triangle_geometry));
    }

    platform_free_file_data(&file);

    return HasSkeleton ? mesh_file_result::is_animated : mesh_file_result::is_static;
}