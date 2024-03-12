#include "Resource_Manager.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/String.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Memory/MemoryUtils.h"

#include "Engine/Renderer/Renderer.h"

#include "Engine/Collision/Collision.h"

#include "Engine/Resources/Filetype/mesh_file_reader.h"

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

RHAPI bool32 resource_load_mesh(const char* resource_file_name, resource_mesh *mesh) {
    memory_arena* arena = resource_get_arena();

    mesh_file *file_data;
    mesh_file_result res = parse_mesh_file(resource_file_name, &file_data, arena);
    if (res == mesh_file_result::error) {
        RH_ERROR("Failed to read data from mesh file.");
        return false;
    }

    mesh->num_primitives = file_data->Header.NumPrims;
    mesh->transform = laml::Mat4(1.0f);

    // buffer primitives to gpu
    mesh->primitives = PushArray(arena, render_geometry, mesh->num_primitives);
    for (int n = 0; n < file_data->Header.NumPrims; n++) {
        mesh_file_primitive *prim = &file_data->Primitives[n];
        renderer_create_mesh(&mesh->primitives[n], prim->Header.NumVerts, 
                             res==mesh_file_result::is_static ? (void*)prim->StaticVertices : (void*)prim->SkinnedVertices, 
                             prim->Header.NumInds, prim->Indices, 
                             res==mesh_file_result::is_static ? static_mesh_attribute_list : dynamic_mesh_attribute_list);
    }

    // buffer textures to gpu
    mesh->materials = PushArray(arena, render_material, mesh->num_primitives);
    for (int n = 0; n < file_data->Header.NumPrims; n++) {
        mesh_file_material *mat = &file_data->Materials[n];
        
        mesh->materials[n].flag = mat->Header.Flag;

        mesh->materials[n].DiffuseFactor = laml::Vec3(mat->Header.DiffuseFactor);
        mesh->materials[n].NormalScale = mat->Header.NormalScale;
        mesh->materials[n].AmbientStrength = mat->Header.AmbientStrength;
        mesh->materials[n].MetallicFactor = mat->Header.MetallicFactor;
        mesh->materials[n].RoughnessFactor = mat->Header.RoughnessFactor;
        mesh->materials[n].EmissiveFactor = laml::Vec3(mat->Header.EmissiveFactor);

        if (mat->Header.Flag & 0x01) {
            // is double-sided I think?
        }
        char tex_path[256];
        if (mat->Header.Flag & 0x02) {
            string_build(tex_path, 256, "Data/textures/%s", mat->DiffuseTexture.str);
            resource_load_texture_file(tex_path, &mesh->materials[n].DiffuseTexture);
        } else {
            //resource_load_texture_file("Data/textures/checker.png", &mesh->materials[n].DiffuseTexture);
        }
        if (mat->Header.Flag & 0x04) {
            string_build(tex_path, 256, "Data/textures/%s", mat->NormalTexture.str);
            resource_load_texture_file(tex_path, &mesh->materials[n].NormalTexture);
        }
        if (mat->Header.Flag & 0x08) {
            string_build(tex_path, 256, "Data/textures/%s", mat->AMRTexture.str);
            resource_load_texture_file(tex_path, &mesh->materials[n].AMRTexture);
        }
        if (mat->Header.Flag & 0x10) {
            string_build(tex_path, 256, "Data/textures/%s", mat->EmissiveTexture.str);
            resource_load_texture_file(tex_path, &mesh->materials[n].EmissiveTexture);
        }
    }

    return false;
}

mesh_file_result resource_load_mesh_file(const char* resource_file_name, 
                                         render_geometry* out_mesh,
                                         skeleton* out_skeleton,
                                         animation** out_animations, uint32* out_num_anims) {
    Assert(!"Don't use this function!!!");
    return mesh_file_result::error;
#if 0
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
    render_geometry* geom = PushStruct(arena, render_geometry);
    if (HasSkeleton) {
        renderer_create_mesh(geom, Header->NumVerts, VertexData, Header->NumInds, Indices, dynamic_mesh_attribute_list);
    } else {
        renderer_create_mesh(geom, Header->NumVerts, VertexData, Header->NumInds, Indices, static_mesh_attribute_list);
    }

    // save to output
    if (out_mesh) {
        memory_copy(out_mesh, geom, sizeof(render_geometry));
    }

    platform_free_file_data(&file);

    return HasSkeleton ? mesh_file_result::is_animated : mesh_file_result::is_static;
    #endif
}


bool32 resource_load_anim_file(animation* anim) {
    return false;
}

bool32 resource_load_mesh(mesh_file * file_data, render_geometry* geom_out) {
    memory_arena* arena = resource_get_arena();

    // Pass onto the renderer to create
    render_geometry* geom = PushStruct(arena, render_geometry);
    renderer_create_mesh(geom, 
                         file_data->Primitives[0].Header.NumVerts, file_data->Primitives[0].StaticVertices, 
                         file_data->Primitives[0].Header.NumInds, file_data->Primitives[0].Indices, 
                         static_mesh_attribute_list);

    // save to output
    if (geom_out) {
        memory_copy(geom_out, geom, sizeof(render_geometry));
    }

    return true;
}
internal_func laml::Vec3 transform_helper(const laml::Mat4& transform, const laml::Vec3 P) {
    laml::Vec4 x(P.x, P.y, P.z, 1.0);
    laml::Vec4 xp = laml::transform::transform_point(transform, x);

    return laml::Vec3(xp.x, xp.y, xp.z);
}
RHAPI bool32 resource_load_mesh_into_grid(mesh_file * file_data, collision_grid * grid, const laml::Mat4& transform) {
    memory_arena* arena = resource_get_arena();

    uint32 num_prims = file_data->Header.NumPrims;
    RH_TRACE("Reserving level triangles to grid!");
    for (uint32 prim_no = 0; prim_no < num_prims; prim_no++) {
        RH_TRACE("  Primitive %d/%d", prim_no, num_prims);
        mesh_file_primitive *prim = &file_data->Primitives[prim_no];

        uint32 num_tris = prim->Header.NumInds / 3;
        uint32 *Indices = prim->Indices;
        mesh_file_vertex_static *Vertices = prim->StaticVertices;
        memory_index arena_save = arena->Used;
        collision_triangle triangle;
        for (uint32 n = 0; n < num_tris; n++) {
            RH_TRACE("    Triangle %d/%d", n, num_tris);
            uint32 I0 = Indices[n*3 + 0];
            uint32 I1 = Indices[n*3 + 1];
            uint32 I2 = Indices[n*3 + 2];

            triangle.v1 = transform_helper(transform, Vertices[I0].Position);
            triangle.v2 = transform_helper(transform, Vertices[I1].Position);
            triangle.v3 = transform_helper(transform, Vertices[I2].Position);

            collision_grid_add_triangle(arena, grid, triangle, true);
        }
    }
    RH_TRACE("Commiting level triangles to grid!");
    for (uint32 prim_no = 0; prim_no < num_prims; prim_no++) {
        RH_TRACE("  Primitive %d/%d", prim_no, num_prims);
        mesh_file_primitive *prim = &file_data->Primitives[prim_no];

        uint32 num_tris = prim->Header.NumInds / 3;
        uint32 *Indices = prim->Indices;
        mesh_file_vertex_static *Vertices = prim->StaticVertices;

        collision_triangle triangle;
        for (uint32 n = 0; n < num_tris; n++) {
            RH_TRACE("    Triangle %d/%d", n, num_tris);
            uint32 I0 = Indices[n*3 + 0];
            uint32 I1 = Indices[n*3 + 1];
            uint32 I2 = Indices[n*3 + 2];

            triangle.v1 = transform_helper(transform, Vertices[I0].Position);
            triangle.v2 = transform_helper(transform, Vertices[I1].Position);
            triangle.v3 = transform_helper(transform, Vertices[I2].Position);

            collision_grid_add_triangle(arena, grid, triangle, false);
        }
    }
    RH_TRACE("Prim Done!");

    return true;
}
bool32 resource_load_mesh_file_for_level(const char* resource_file_name,
                                                   render_geometry* out_geom,
                                                   collision_grid* grid,
                                                   laml::Mat4 transform) {

    memory_arena* arena = resource_get_arena();

    mesh_file *file_data;
    mesh_file_result res = parse_mesh_file(resource_file_name, &file_data, arena);
    if (res != mesh_file_result::is_static) {
        RH_ERROR("Failed to read data from mesh file.");
        return false;
    }

    uint32 num_prims = file_data->Header.NumPrims;
    RH_TRACE("Reserving level triangles to grid!");
    for (uint32 prim_no = 0; prim_no < num_prims; prim_no++) {
        RH_TRACE("  Primitive %d/%d", prim_no, num_prims);
        mesh_file_primitive *prim = &file_data->Primitives[prim_no];

        uint32 num_tris = prim->Header.NumInds / 3;
        uint32 *Indices = prim->Indices;
        mesh_file_vertex_static *Vertices = prim->StaticVertices;
        memory_index arena_save = arena->Used;
        collision_triangle triangle;
        for (uint32 n = 0; n < num_tris; n++) {
            RH_TRACE("    Triangle %d/%d", n, num_tris);
            uint32 I0 = Indices[n*3 + 0];
            uint32 I1 = Indices[n*3 + 1];
            uint32 I2 = Indices[n*3 + 2];

            triangle.v1 = transform_helper(transform, Vertices[I0].Position);
            triangle.v2 = transform_helper(transform, Vertices[I1].Position);
            triangle.v3 = transform_helper(transform, Vertices[I2].Position);

            collision_grid_add_triangle(arena, grid, triangle, true);
        }
    }
    RH_TRACE("Commiting level triangles to grid!");
    for (uint32 prim_no = 0; prim_no < num_prims; prim_no++) {
        RH_TRACE("  Primitive %d/%d", prim_no, num_prims);
        mesh_file_primitive *prim = &file_data->Primitives[prim_no];

        uint32 num_tris = prim->Header.NumInds / 3;
        uint32 *Indices = prim->Indices;
        mesh_file_vertex_static *Vertices = prim->StaticVertices;

        collision_triangle triangle;
        for (uint32 n = 0; n < num_tris; n++) {
            RH_TRACE("    Triangle %d/%d", n, num_tris);
            uint32 I0 = Indices[n*3 + 0];
            uint32 I1 = Indices[n*3 + 1];
            uint32 I2 = Indices[n*3 + 2];

            triangle.v1 = transform_helper(transform, Vertices[I0].Position);
            triangle.v2 = transform_helper(transform, Vertices[I1].Position);
            triangle.v3 = transform_helper(transform, Vertices[I2].Position);

            collision_grid_add_triangle(arena, grid, triangle, false);
        }
    }
    RH_TRACE("Prim Done!");

    // Pass onto the renderer to create
    render_geometry* geom = PushStruct(arena, render_geometry);
    renderer_create_mesh(geom, 
                         file_data->Primitives[0].Header.NumVerts, file_data->Primitives[0].StaticVertices, 
                         file_data->Primitives[0].Header.NumInds, file_data->Primitives[0].Indices, 
                         static_mesh_attribute_list);

    // save to output
    if (out_geom) {
        memory_copy(out_geom, geom, sizeof(render_geometry));
    }

    return false;
}