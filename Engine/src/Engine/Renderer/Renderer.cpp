#include "Renderer.h"

#include "Renderer_API.h"
#include "Engine/Core/Logger.h"

#include <stdarg.h>

global_variable renderer_api* backend;


bool32 renderer_initialize(memory_arena* arena, const char* application_name, platform_state* plat_state) {
    if (!renderer_api_create(arena, RENDERER_API_OPENGL, plat_state, &backend)) {
        return false;
    }

    if (!backend->initialize(application_name, plat_state)) {
        RH_FATAL("Renderer API failed to initialize!");
        return false;
    }

    return true;
}

void renderer_shutdown() {
    renderer_api_destroy(backend);
    backend = 0;
}

bool32 renderer_begin_Frame(real32 delta_time) {
    return backend->begin_frame(delta_time);
}
bool32 renderer_end_Frame(real32 delta_time) {
    bool32 result = backend->end_frame(delta_time);
    backend->frame_number++;
    return result;
}

bool32 renderer_draw_frame(render_packet* packet) {
    if (renderer_begin_Frame(packet->delta_time)) {
        // render all commands in the packet

        

        bool32 result = renderer_end_Frame(packet->delta_time);

        if (!result) {
            RH_ERROR("end_frame() failed for some reason!");
            return false;
        }
    }

    return true;
}

void renderer_resized(uint16 width, uint16 height) {
    // resize framebuffers
}





void renderer_create_texture(struct texture_2D* texture, const uint8* data) {
    backend->create_texture(texture, data);
}
void renderer_destroy_texture(struct texture_2D* texture) {
    backend->destroy_texture(texture);
}

void renderer_create_mesh(triangle_geometry* mesh, 
                          uint32 num_verts, const void* vertices,
                          uint32 num_inds, const uint32* indices,
                          const ShaderDataType* attributes) {
    backend->create_mesh(mesh, 
                         num_verts, vertices, 
                         num_inds, indices, 
                         attributes);
}
void renderer_destroy_mesh(triangle_geometry* mesh) {
    backend->destroy_mesh(mesh);
}

void renderer_shader_create(struct shader* shader_prog, const char* shader_source) {
    backend->shader_create(shader_prog, shader_source);
}
void renderer_shader_destroy(struct shader* shader_prog) {
    backend->shader_destroy(shader_prog);
}