#include "Resource_Manager.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/String.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Memory/Memory.h"

#include "Engine/Renderer/Renderer.h"

#include "Engine/Collision/Collision.h"

#include "Engine/Resources/Filetype/mesh_file_reader.h"

bool32 resource_load_static_mesh(const char* resource_file_name, resource_static_mesh* mesh) {
    memory_arena* arena = resource_get_arena();

    mesh_file* file_data;
    mesh_file_result res = parse_mesh_file(resource_file_name, &file_data, arena);
    if (res == mesh_file_result::error) {
        RH_ERROR("Failed to read data from mesh file.");
        return false;
    }

    const ShaderDataType* attribs = static_mesh_attribute_list;

    if (res == mesh_file_result::is_skinned) {
        // downgrade a skinned mesh to a static mesh...
        RH_WARN("Requested a static_mesh but '%s' is a skinned_mesh.", resource_file_name);
        attribs = dynamic_mesh_attribute_list;
    }

    mesh->num_primitives = file_data->Header.NumPrims;

    // buffer primitives to gpu
    mesh->primitives = PushArray(arena, render_geometry, mesh->num_primitives);
    for (int n = 0; n < file_data->Header.NumPrims; n++) {
        mesh_file_primitive *prim = &file_data->Primitives[n];
        renderer_create_mesh(&mesh->primitives[n], prim->Header.NumVerts, 
                             prim->Vertices, 
                             prim->Header.NumInds, prim->Indices, 
                             attribs);
    }

    // buffer textures to gpu
    mesh->materials = PushArray(arena, resource_material, mesh->num_primitives);
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

bool32 resource_load_skinned_mesh(const char* resource_file_name, resource_skinned_mesh* mesh) {
    memory_arena* arena = resource_get_arena();

    mesh_file* file_data;
    mesh_file_result res = parse_mesh_file(resource_file_name, &file_data, arena);
    if (res == mesh_file_result::error) {
        RH_ERROR("Failed to read data from mesh file.");
        return false;
    }

    const ShaderDataType* attribs = dynamic_mesh_attribute_list;

    if (res == mesh_file_result::is_static) {
        // asking for a skinned mesh but no bones present...
        RH_WARN("Requested a static_mesh but '%s' is a skinned_mesh.", resource_file_name);
        return false;
    }

    mesh->num_primitives = file_data->Header.NumPrims;

    // buffer primitives to gpu
    mesh->primitives = PushArray(arena, render_geometry, mesh->num_primitives);
    for (int n = 0; n < file_data->Header.NumPrims; n++) {
        mesh_file_primitive *prim = &file_data->Primitives[n];
        renderer_create_mesh(&mesh->primitives[n], prim->Header.NumVerts, 
                             prim->Vertices, 
                             prim->Header.NumInds, prim->Indices, 
                             attribs);
    }

    // buffer textures to gpu
    mesh->materials = PushArray(arena, resource_material, mesh->num_primitives);
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

    // parse skeleton data
    mesh->skeleton.num_bones = file_data->Skeleton.Header.NumBones;
    mesh->skeleton.bones = PushArray(arena, resource_bone, mesh->skeleton.num_bones);
    for (uint32 b = 0; b < mesh->skeleton.num_bones; b++) {
        resource_bone& bone = mesh->skeleton.bones[b];

        bone.bone_idx         = file_data->Skeleton.Bones[b].bone_idx;
        bone.parent_idx       = file_data->Skeleton.Bones[b].parent_idx;
        bone.local_matrix     = file_data->Skeleton.Bones[b].local_matrix;
        bone.inv_model_matrix = file_data->Skeleton.Bones[b].inv_model_matrix;
        bone.debug_length     = file_data->Skeleton.Bones[b].debug_length;

        uint8 name_len = file_data->Skeleton.Bones[b].name.len;
        bone.debug_name = PushArray(arena, char, name_len+1);
        memory_copy(bone.debug_name, file_data->Skeleton.Bones[b].name.str, name_len + 1);
        bone.debug_name[name_len] = 0; // null terminate

        RH_INFO("Bone: [%2d | %2d] '%s'", bone.bone_idx, bone.parent_idx, bone.debug_name);
    }

    return false;
}