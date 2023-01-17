#pragma once

#include "../Renderer_API.h"

struct OpenGL_api : public renderer_api {
    bool32 initialize(const char* application_name, struct platform_state* plat_state) override;
    void shutdown() override;

    void resized(uint16 width, uint16 height) override;

    bool32 begin_frame(real32 delta_time) override;
    bool32 end_frame(real32 delta_time) override;


    void create_texture(struct texture_2D* texture, const uint8* data) override;
    void destroy_texture(struct texture_2D* texture) override;
    void create_mesh(struct triangle_mesh* mesh, uint32 num_verts, const void* vertices,uint32 num_inds, const uint32* indices) override;
    void destroy_mesh(struct triangle_mesh* mesh) override;
    void shader_create(struct shader* shader_prog, const char* shader_source) override;
    void shader_destroy(struct shader* shader_prog) override;
};