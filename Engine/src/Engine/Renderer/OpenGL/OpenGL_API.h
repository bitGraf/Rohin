#pragma once

#include "../Renderer_API.h"

struct OpenGL_api final : public renderer_api {
    bool32 initialize(const char* application_name, struct platform_state* plat_state) override final;
    void shutdown() override final;

    void resized(uint16 width, uint16 height) override final;

    bool32 begin_frame(real32 delta_time) override final;
    bool32 end_frame(real32 delta_time) override final;


    void create_texture(struct texture_2D* texture, const uint8* data) override final;
    void destroy_texture(struct texture_2D* texture) override final;
    void create_mesh(triangle_geometry* mesh, 
                     uint32 num_verts, const void* vertices,
                     uint32 num_inds, const uint32* indices,
                     const ShaderDataType* attributes) override final;
    void destroy_mesh(triangle_geometry* mesh) override final;
    void create_shader(struct shader* shader_prog, const char* shader_source) override final;
    void destroy_shader(struct shader* shader_prog) override final;
};