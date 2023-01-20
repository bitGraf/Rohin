#include "Resource_Manager.h"

#include "Engine/Core/Logger.h"
#include "Engine/Platform/Platform.h"

#include <cstring>

bool32 resource_load_static_mesh(static_mesh* mesh, const char* resource_name) {
    char full_path[256];
    platform_get_full_resource_path(full_path, 256, resource_name);

    RH_DEBUG("Fulle filename: [%s]", full_path);

    file_handle file = platform_read_entire_file(full_path);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");
        return false;
    }

    platform_free_file_data(&file);

    return true;
}