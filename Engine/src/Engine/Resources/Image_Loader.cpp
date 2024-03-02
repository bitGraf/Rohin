#include "Resource_Manager.h"

#include "Engine/Core/Logger.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Renderer/Renderer.h"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

bool32 resource_load_texture_file(const char* resource_file_name,
                                  render_texture_2D* texture) {

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
    renderer_create_texture(texture, bitmap, false);

    stbi_image_free(bitmap);

    return true;
}


bool32 resource_load_texture_file_hdr(const char* resource_file_name,
                                      render_texture_2D* texture) {

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
    renderer_create_texture(texture, data, true);

    stbi_image_free(data);

    return true;
}