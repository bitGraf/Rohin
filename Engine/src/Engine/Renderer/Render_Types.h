#pragma once

#include "Engine/Defines.h"
#include <laml/laml.hpp>

struct memory_arena;

enum renderer_api_type {
    RENDERER_API_OPENGL,
};

struct renderer_api {
    //struct platform_state* plat_state; // platform-specific state
    uint64 frame_number;

    virtual bool32 initialize(const char* application_name, struct platform_state* plat_state) = 0;
    virtual void shutdown() = 0;
    virtual void resized(uint16 width, uint16 height) = 0;

    virtual bool32 begin_frame(real32 delta_time) = 0;
    virtual bool32 end_frame(real32 delta_time) = 0;

    virtual void create_texture(struct texture_2D* texture, const uint8* data) = 0;
    virtual void destroy_texture(struct texture_2D* texture) = 0;
    virtual void create_mesh(struct triangle_mesh* mesh, uint32 num_verts, const void* vertices,uint32 num_inds, const uint32* indices) = 0;
    virtual void destroy_mesh(struct triangle_mesh* mesh) = 0;
    virtual void shader_create(struct shader* shader_prog, const char* shader_source_filename) = 0;
    virtual void shader_destroy(struct shader* shader_prog) = 0;
};

struct render_command {
    laml::Mat4 model_matrix;
    uint32 mesh_handle;
    uint32 material_handle;
};

struct render_packet {
    memory_arena* arena;

    real32 delta_time;

    laml::Mat4 projection_matrix;
    laml::Mat4 view_matrix;
    laml::Vec3 camera_pos;

    uint32 num_commands;
    render_command* commands;
};