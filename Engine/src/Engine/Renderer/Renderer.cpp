#include "Renderer.h"

#include "Renderer_API.h"
#include "Engine/Core/Logger.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Core/Asserts.h"

#include "Engine/Resources/Resource_Manager.h"

#include <stdarg.h>

struct renderer_state {
    shader simple_shader;
};

global_variable renderer_api* backend;
global_variable renderer_state* render_state;

bool32 renderer_initialize(memory_arena* arena, const char* application_name, platform_state* plat_state) {
    if (!renderer_api_create(arena, RENDERER_API_OPENGL, plat_state, &backend)) {
        return false;
    }

    if (!backend->initialize(application_name, plat_state)) {
        RH_FATAL("Renderer API failed to initialize!");
        return false;
    }

    render_state = PushStruct(arena, renderer_state);

    return true;
}

bool32 renderer_create_pipeline() {
    Assert(render_state);

    if (!resource_load_shader_file("Data/Shaders/simple.glsl", &render_state->simple_shader)) {
        RH_FATAL("Could not setup the main shader");
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

        renderer_use_shader(&render_state->simple_shader);

        laml::Mat4 proj_view = laml::mul(packet->projection_matrix, packet->view_matrix);
        renderer_upload_uniform_float4x4(&render_state->simple_shader, "r_VP", proj_view._data);
        laml::Vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
        renderer_upload_uniform_float4(&render_state->simple_shader, "u_color", color._data);
        renderer_upload_uniform_int(&render_state->simple_shader, "u_texture", 0);

        for (uint32 cmd_index = 0; cmd_index < packet->num_commands; cmd_index++) {
            renderer_upload_uniform_float4x4(&render_state->simple_shader, "r_Transform", 
                                             packet->commands[cmd_index].model_matrix._data);
            renderer_draw_geometry(&packet->commands[cmd_index].geom);
        }

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

bool32 renderer_create_shader(shader* shader_prog, const uint8* shader_source, uint64 num_bytes) {
    return backend->create_shader(shader_prog, shader_source, num_bytes);
}
void renderer_destroy_shader(shader* shader_prog) {
    backend->destroy_shader(shader_prog);
}


void renderer_use_shader(shader* shader_prog) {
    backend->use_shader(shader_prog);
}
void renderer_draw_geometry(triangle_geometry* geom) {
    backend->draw_geometry(geom);
}

void renderer_upload_uniform_float(shader* shader_prog, const char* uniform_name, float value) {
    backend->upload_uniform_float(shader_prog, uniform_name, value);
}
void renderer_upload_uniform_float2(shader* shader_prog, const char* uniform_name, float* values) {
    backend->upload_uniform_float2(shader_prog, uniform_name, values);
}
void renderer_upload_uniform_float3(shader* shader_prog, const char* uniform_name, float* values) {
    backend->upload_uniform_float3(shader_prog, uniform_name, values);
}
void renderer_upload_uniform_float4(shader* shader_prog, const char* uniform_name, float* values) {
    backend->upload_uniform_float4(shader_prog, uniform_name, values);
}
void renderer_upload_uniform_float4x4(shader* shader_prog, const char* uniform_name, float* values) {
    backend->upload_uniform_float4x4(shader_prog, uniform_name, values);
}
void renderer_upload_uniform_int(shader* shader_prog, const char* uniform_name, int  value) {
    backend->upload_uniform_int(shader_prog, uniform_name, value);
}
void renderer_upload_uniform_int2(shader* shader_prog, const char* uniform_name, int* values) {
    backend->upload_uniform_int2(shader_prog, uniform_name, values);
}
void renderer_upload_uniform_int3(shader* shader_prog, const char* uniform_name, int* values) {
    backend->upload_uniform_int3(shader_prog, uniform_name, values);
}
void renderer_upload_uniform_int4(shader* shader_prog, const char* uniform_name, int* values) {
    backend->upload_uniform_int4(shader_prog, uniform_name, values);
}