#include "Renderer.h"

#include "Renderer_API.h"
#include "Engine/Core/Logger.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Core/Asserts.h"

#include "Engine/Resources/Resource_Manager.h"

#include "Engine/Collision/Collision.h"

#include <stdarg.h>

struct renderer_state {
    uint32 render_width;
    uint32 render_height;

    // simple render pass
    shader simple_shader;

    // deferred pbr render pass
    shader pre_pass_shader;

    // framebuffer
    frame_buffer gbuffer;

    // debug wireframe
    shader wireframe_shader;
    triangle_geometry cube_geom;
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
    render_state->render_height = 0;
    render_state->render_width = 0;

    return true;
}

bool32 renderer_create_pipeline() {
    Assert(render_state);

    // setup simple shader
    //if (!resource_load_shader_file("Data/Shaders/simple.glsl", &render_state->simple_shader)) {
    if (!resource_load_shader_file("Data/Shaders/simple_triplanar.glsl", &render_state->simple_shader)) {
        RH_FATAL("Could not setup the main shader");
        return false;
    }
    renderer_use_shader(&render_state->simple_shader);
    renderer_upload_uniform_int(&render_state->simple_shader, "u_texture", 0);

    // setup prepass shader
    if (!resource_load_shader_file("Data/Shaders/PrePass.glsl", &render_state->pre_pass_shader)) {
        RH_FATAL("Could not setup the pre-pass shader");
        return false;
    }
    renderer_use_shader(&render_state->pre_pass_shader);
    renderer_upload_uniform_int(&render_state->pre_pass_shader, "u_AlbedoTexture", 0);
    renderer_upload_uniform_int(&render_state->pre_pass_shader, "u_NormalTexture", 1);
    renderer_upload_uniform_int(&render_state->pre_pass_shader, "u_MetalnessTexture", 2);
    renderer_upload_uniform_int(&render_state->pre_pass_shader, "u_RoughnessTexture", 3);
    renderer_upload_uniform_int(&render_state->pre_pass_shader, "u_AmbientTexture", 4);
    renderer_upload_uniform_int(&render_state->pre_pass_shader, "u_EmissiveTexture", 5);

    // setup wireframe shader
    if (!resource_load_shader_file("Data/Shaders/wireframe.glsl", &render_state->wireframe_shader)) {
        RH_FATAL("Could not setup the wireframe shader");
        return false;
    }
    renderer_use_shader(&render_state->pre_pass_shader);

    // create g-buffer
    laml::Vec4 black;
    laml::Vec4 _dist(100.0f, 100.0f, 100.0f, 1.0f);
    frame_buffer_attachment attachments[] = {
        {0, frame_buffer_texture_format::RGBA8,   black}, // Albedo
        {0, frame_buffer_texture_format::RGBA16F, black}, // View-space normal
        {0, frame_buffer_texture_format::RGBA8,   black}, // Ambient/Metallic/Roughness
        {0, frame_buffer_texture_format::RGBA16F, _dist},  // Distance
        {0, frame_buffer_texture_format::RGBA8,   black},  // Emissive
        {0, frame_buffer_texture_format::Depth,   black}  // Depth-buffer
    };
    renderer_create_framebuffer(&render_state->gbuffer, 6, attachments);

    // create cube mesh for debug purposes
    real32 s = 1.0f;
    typedef laml::Vec3 col_grid_vert;
    col_grid_vert cube_verts[] = {
                                 {0, 0, 0},// 0
                                 {s, 0, 0},// 1
                                 {s, s, 0},// 2
                                 {0, s, 0},// 3

                                 {0, 0, s}, // 4
                                 {s, 0, s}, // 5
                                 {s, s, s}, // 6
                                 {0, s, s}, // 7
    };
    uint32 cube_inds[] = {
                         // bottom face
                         0, 1,
                         1, 2,
                         2, 3,
                         3, 0,

                         // top face
                         4, 5,
                         5, 6,
                         6, 7,
                         7, 4,

                         //verticals
                         0, 4,
                         1, 5,
                         2, 6,
                         3, 7
    };
    const ShaderDataType cube_attrs[] = {ShaderDataType::Float3, ShaderDataType::None};
    backend->create_mesh(&render_state->cube_geom, 8, cube_verts, 36, cube_inds, cube_attrs);

    return true;
}

void renderer_shutdown() {
    renderer_api_destroy(backend);
    backend = 0;
}

bool32 renderer_begin_Frame(real32 delta_time) {
    if (!backend->begin_frame(delta_time)) {
        return false;
    }

    backend->set_viewport(0, 0, render_state->render_width, render_state->render_height);
    backend->clear_viewport(0, 0, 0, 0);

    return true;
}
bool32 renderer_end_Frame(real32 delta_time) {
    bool32 result = backend->end_frame(delta_time);
    backend->frame_number++;
    return result;
}

#define SIMPLE_RENDER_PASS 1
bool32 renderer_draw_frame(render_packet* packet) {
    if (renderer_begin_Frame(packet->delta_time)) {
        // render all commands in the packet

        laml::Mat4 cam_transform;
        laml::transform::create_transform(cam_transform, packet->camera_orientation, packet->camera_pos);
        laml::transform::create_view_matrix_from_transform(packet->view_matrix, cam_transform);

#if SIMPLE_RENDER_PASS
        renderer_use_shader(&render_state->simple_shader);

        laml::Mat4 proj_view = laml::mul(packet->projection_matrix, packet->view_matrix);
        renderer_upload_uniform_float4x4(&render_state->simple_shader, "r_VP", proj_view._data);
        laml::Vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
        renderer_upload_uniform_float4(&render_state->simple_shader, "u_color", color._data);

        for (uint32 cmd_index = 0; cmd_index < packet->num_commands; cmd_index++) {
            renderer_upload_uniform_float4x4(&render_state->simple_shader, "r_Transform", 
                                             packet->commands[cmd_index].model_matrix._data);
            renderer_draw_geometry(&packet->commands[cmd_index].geom);
        }
#else
        renderer_use_shader(&render_state->pre_pass_shader);

        renderer_upload_uniform_float4x4(&render_state->pre_pass_shader, "r_View", packet->view_matrix._data);
        renderer_upload_uniform_float4x4(&render_state->pre_pass_shader, "r_Projection", packet->projection_matrix._data);
        laml::Vec3 color(1.0f, 0.0f, 1.0f);
        renderer_upload_uniform_float3(&render_state->pre_pass_shader, "u_AlbedoColor", color._data);
        renderer_upload_uniform_float(&render_state->pre_pass_shader, "u_Metalness", 0.0f);
        renderer_upload_uniform_float(&render_state->pre_pass_shader, "u_Roughness", 0.5f);
        renderer_upload_uniform_float(&render_state->pre_pass_shader, "u_TextureScale", 1.0f);

        renderer_upload_uniform_float(&render_state->pre_pass_shader, "r_AlbedoTexToggle", 1.0f);
        renderer_upload_uniform_float(&render_state->pre_pass_shader, "r_NormalTexToggle", 0.0f);
        renderer_upload_uniform_float(&render_state->pre_pass_shader, "r_MetalnessTexToggle", 0.0f);
        renderer_upload_uniform_float(&render_state->pre_pass_shader, "r_RoughnessTexToggle", 0.0f);
        renderer_upload_uniform_float(&render_state->pre_pass_shader, "r_AmbientTexToggle", 0.0f);
        renderer_upload_uniform_float(&render_state->pre_pass_shader, "r_EmissiveTexToggle", 0.0f);
        renderer_upload_uniform_float(&render_state->pre_pass_shader, "r_gammaCorrect", 1.0f);

        for (uint32 cmd_index = 0; cmd_index < packet->num_commands; cmd_index++) {
            renderer_upload_uniform_float4x4(&render_state->pre_pass_shader, "r_Transform", 
                                             packet->commands[cmd_index].model_matrix._data);
            renderer_draw_geometry(&packet->commands[cmd_index].geom);
        }
#endif

#if 1
        // Render debug wireframes
        renderer_begin_wireframe();
        renderer_use_shader(&render_state->wireframe_shader);

        //laml::Mat4 proj_view = laml::mul(packet->projection_matrix, packet->view_matrix);
        renderer_upload_uniform_float4x4(&render_state->wireframe_shader, "r_VP", proj_view._data);
        laml::Vec4 wire_color(.6f, 0.5f, 0.65f, 1.0f);
        renderer_upload_uniform_float4(&render_state->wireframe_shader, "u_color", wire_color._data);

        for (uint32 cmd_index = 0; cmd_index < packet->num_commands; cmd_index++) {
            renderer_upload_uniform_float4x4(&render_state->wireframe_shader, "r_Transform",
                                             packet->commands[cmd_index].model_matrix._data);
            renderer_draw_geometry(&packet->commands[cmd_index].geom);
            
            laml::Vec4 point_color = { .8f, 0.4f, 0.25f, 1.0f };
            renderer_upload_uniform_float4(&render_state->wireframe_shader, "u_color", point_color._data);
            renderer_draw_geometry_points(&packet->commands[cmd_index].geom);
        }

        renderer_end_wireframe();

        if (packet->col_grid && packet->col_grid->num_filled_cells > 0) {
            collision_grid* grid = packet->col_grid;

            real32 min_x = -((real32)(grid->num_x / 2)) * grid->cell_size;
            real32 min_y = -((real32)(grid->num_y / 2)) * grid->cell_size;
            real32 min_z = -((real32)(grid->num_z / 2)) * grid->cell_size;

            real32 max_x = ((real32)(grid->num_x / 2)) * grid->cell_size;
            real32 max_y = ((real32)(grid->num_y / 2)) * grid->cell_size;
            real32 max_z = ((real32)(grid->num_z / 2)) * grid->cell_size;

            laml::Vec3 min_cell(min_x, min_y, min_z);
            laml::Vec3 max_cell(max_x, max_y, max_z);

            laml::Vec3 cell_offset(grid->cell_size);

            laml::Mat4 cell_transform(1.0f);
            renderer_upload_uniform_float4x4(&render_state->wireframe_shader, "r_Transform",
                                             cell_transform._data);

            wire_color = { .7f, 0.6f, 0.35f, 1.0f };
            renderer_upload_uniform_float4(&render_state->wireframe_shader, "u_color", wire_color._data);
            renderer_draw_geometry_lines(&packet->col_grid->geom);
            //wire_color = { .8f, 0.4f, 0.25f, 1.0f };
            //renderer_upload_uniform_float4(&render_state->wireframe_shader, "u_color", wire_color._data);
            //renderer_draw_geometry_points(&packet->col_grid->geom);
        }

        // Draw the highlighted cube/faces
        if (packet->col_grid) {
            backend->set_highlight_mode(true);

            collision_grid* grid = packet->col_grid;

            // render sector of cells
            for (int32 x = packet->sector.x_min; x <= packet->sector.x_max; x++) {
                for (int32 y = packet->sector.y_min; y <= packet->sector.y_max; y++) {
                    for (int32 z = packet->sector.z_min; z <= packet->sector.z_max; z++) {
                        collision_grid_cell* cell = &packet->col_grid->cells[x][y][z];

                        laml::Vec3 cell_pos = collision_cell_to_world(packet->col_grid, x, y, z);
                        laml::Mat4 cell_transform;
                        laml::transform::create_transform_translate(cell_transform, cell_pos);
                        laml::Mat4 scale_m;
                        laml::transform::create_transform_scale(scale_m, laml::Vec3(grid->cell_size));
                        cell_transform = laml::mul(cell_transform, scale_m);
                        renderer_upload_uniform_float4x4(&render_state->wireframe_shader, "r_Transform",
                                                         cell_transform._data);
                        wire_color = { .35f, 0.6f, 0.7f, 0.5f };
                        renderer_upload_uniform_float4(&render_state->wireframe_shader, "u_color", wire_color._data);

                        backend->disable_depth_test();
                        renderer_draw_geometry_lines(&render_state->cube_geom);
                        backend->enable_depth_test();
                    }
                }
            }

#if 0
            if (cell->num_surfaces > 0) {
                renderer_use_shader(&render_state->simple_shader);

                color = laml::Vec4(1.0f, 0.2f, 0.5f, 1.0f);
                renderer_upload_uniform_float4(&render_state->simple_shader, "u_color", color._data);

                for (uint32 n = 0; n < cell->num_surfaces; n++) {
                    uint32 tri_idx = cell->surfaces[n];

                    uint32 start_idx = tri_idx * 3;
                    renderer_upload_uniform_float4x4(&render_state->simple_shader, "r_Transform", 
                                                     packet->commands[0].model_matrix._data);
                    renderer_draw_geometry(&packet->commands[0].geom, start_idx, 3);
                }
            }
#endif

            backend->set_highlight_mode(false);
        }
#endif

        bool32 result = renderer_end_Frame(packet->delta_time);

        if (!result) {
            RH_ERROR("end_frame() failed for some reason!");
            return false;
        }
    }

    return true;
}

void renderer_resized(uint32 width, uint32 height) {
    if (width != render_state->render_width || height != render_state->render_height) {
        // resize framebuffers
        render_state->render_width = width;
        render_state->render_height = height;
    }
}

bool32 renderer_begin_wireframe() {
    backend->set_draw_mode(render_draw_mode::Wireframe);
    return true;
}
bool32 renderer_end_wireframe() {
    backend->set_draw_mode(render_draw_mode::Normal);
    return true;
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

bool32 renderer_create_framebuffer(frame_buffer* fbo, 
                                   int num_attachments, const frame_buffer_attachment* attachments) {
    return backend->create_framebuffer(fbo, num_attachments, attachments);
}
void renderer_destroy_framebuffer(frame_buffer* fbo) {
    backend->destroy_framebuffer(fbo);
}


void renderer_use_shader(shader* shader_prog) {
    backend->use_shader(shader_prog);
}
void renderer_draw_geometry(triangle_geometry* geom) {
    backend->draw_geometry(geom);
}
void renderer_draw_geometry(triangle_geometry* geom, uint32 start_idx, uint32 num_inds) {
    backend->draw_geometry(geom, start_idx, num_inds);
}
void renderer_draw_geometry_lines(triangle_geometry* geom) {
    backend->draw_geometry_lines(geom);
}
void renderer_draw_geometry_points(triangle_geometry* geom) {
    backend->draw_geometry_points(geom);
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