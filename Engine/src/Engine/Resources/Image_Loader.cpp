#include "Resource_Manager.h"

#include "Engine/Core/Logger.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/String.h"
#include "Engine/Core/Asserts.h"

#include "Engine/Memory/Memory_Arena.h"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

bool32 resource_load_texture_file(const char* resource_file_name,
                                  resource_texture_2D* texture) {

    char full_path[256];
    platform_get_full_resource_path(full_path, 256, resource_file_name);

    RH_DEBUG("Full filename: [%s]", full_path);

    file_handle file = platform_read_entire_file(full_path);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");
        return false;
    }

    memory_arena* arena = resource_get_arena();

    stbi_set_flip_vertically_on_load(true);
    // TODO: This calls malloc under the hood!
    //       can be overwritten with #define STBI_MALLOC ...
    int x,y,n;
    unsigned char *bitmap = stbi_load_from_memory(file.data, (int)file.num_bytes, &x, &y, &n, 0);
    platform_free_file_data(&file);
    if (bitmap == NULL) {
        RH_ERROR("Failed to load image file!");
        return false;
    }

    texture->width = (uint16)x;
    texture->height = (uint16)y;
    texture->num_channels = (uint16)n;
    texture->flag = 0;

    texture_creation_info_2D info;
    info.width = texture->width;
    info.height = texture->height;
    info.num_channels = texture->num_channels;
    renderer_create_texture(&texture->texture, info, bitmap, false);

    stbi_image_free(bitmap);

    return true;
}


bool32 resource_load_texture_file_hdr(const char* resource_file_name,
                                      resource_texture_2D* texture) {

    char full_path[256];
    platform_get_full_resource_path(full_path, 256, resource_file_name);

    RH_DEBUG("Full filename: [%s]", full_path);

    file_handle file = platform_read_entire_file(full_path);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");
        return false;
    }

    memory_arena* arena = resource_get_arena();

    stbi_set_flip_vertically_on_load(true);
    // TODO: This calls malloc under the hood!
    //       can be overwritten with #define STBI_MALLOC ...
    int x,y,n;
    real32 *data = stbi_loadf_from_memory(file.data, (int)file.num_bytes, &x, &y, &n, 0);
    platform_free_file_data(&file);
    if (data == NULL) {
        RH_ERROR("Failed to load image file!");
        return false;
    }

    texture->width = (uint16)x;
    texture->height = (uint16)y;
    texture->num_channels = (uint16)n;
    texture->flag = 0;

    texture_creation_info_2D info;
    info.width = texture->width;
    info.height = texture->height;
    info.num_channels = texture->num_channels;
    renderer_create_texture(&texture->texture, info, data, true);

    stbi_image_free(data);

    return true;
}

bool32 resource_load_texture_debug_cube_map(resource_texture_cube* texture) {
    char* cube_sides[] = {
        "Data/textures/pos_x.png",
        "Data/textures/neg_x.png",
        "Data/textures/pos_y.png",
        "Data/textures/neg_y.png",
        "Data/textures/pos_z.png",
        "Data/textures/neg_z.png",
    };

    char full_path[256];
    memory_arena* arena = resource_get_arena();

    stbi_set_flip_vertically_on_load(true);
    int x[6], y[6], n[6];
    unsigned char** bitmap;
    bitmap = PushArray(arena, unsigned char*, 6);
    for (uint32 i = 0; i < 6; i++) {
        platform_get_full_resource_path(full_path, 256, cube_sides[i]);

        RH_DEBUG("Full filename: [%s]", full_path);

        file_handle file = platform_read_entire_file(full_path);
        if (!file.num_bytes) {
            RH_ERROR("Failed to read resource file");
            return false;
        }

        // TODO: This calls malloc under the hood!
        //       can be overwritten with #define STBI_MALLOC ...
        bitmap[i] = stbi_load_from_memory(file.data, (int)file.num_bytes, &x[i], &y[i], &n[i], 0);
        platform_free_file_data(&file);
        if (bitmap[i] == NULL) {
            RH_ERROR("Failed to load image file!");
            return false;
        }
    }

    texture->width = (uint16)x[0];
    texture->height = (uint16)y[0];
    texture->num_channels = (uint16)n[0];
    texture->flag = 0;

    texture_creation_info_cube info;
    info.width = texture->width;
    info.height = texture->height;
    info.num_channels = texture->num_channels;
    renderer_create_texture_cube(&texture->texture, info, (const void**)(bitmap), false);

    for (uint32 i = 0; i < 6; i++) {
        stbi_image_free(bitmap[i]);
    }

    return true;
}

bool32 resource_load_env_map(const char* resource_file_name,
                             resource_env_map* env_map) {
    char full_path[256];
    platform_get_full_resource_path(full_path, 256, resource_file_name);

    RH_DEBUG("Full filename: [%s]", full_path);

    memory_arena* arena = resource_get_arena();

    // first check if pre-computed data exists (doesn't for now...)
    char baked_path[256];
    string_build(baked_path, 256, "%s_bake", full_path);

    file_info finfo;
    if (platform_get_file_attributes(baked_path, &finfo)) {
        // pre-baked data exists! load meta-data file then load cubemaps into memory
        RH_ERROR("pre-baked files not implemented yet!");
        return false;

    } else {
        // not yet baked, need to generate the environment map data

        file_handle file = platform_read_entire_file(full_path);
        if (!file.num_bytes) {
            RH_ERROR("Failed to read resource file");
            return false;
        }

        stbi_set_flip_vertically_on_load(true);
        // TODO: This calls malloc under the hood!
        //       can be overwritten with #define STBI_MALLOC ...
        int x,y,n;
        real32 *data = stbi_loadf_from_memory(file.data, (int)file.num_bytes, &x, &y, &n, 0);
        platform_free_file_data(&file);
        if (data == NULL) {
            RH_ERROR("Failed to load image file!");
            return false;
        }

        env_map->src.width = (uint16)x;
        env_map->src.height = (uint16)y;
        env_map->src.num_channels = (uint16)n;
        env_map->src.flag = 0;

        env_map->skybox_size     = 1024;
        env_map->irradiance_size = 32;
        env_map->prefilter_size  = 128;

        renderer_precompute_env_map_from_equirectangular(env_map, data);

        stbi_image_free(data);
    }

    return true;
}