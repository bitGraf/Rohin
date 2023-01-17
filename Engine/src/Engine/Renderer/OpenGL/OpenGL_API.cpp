#include "OpenGL_API.h"

#include "Engine/Core/Logger.h"

#include "OpenGL_Types.h"
#include "OpenGL_Platform.h"

bool32 OpenGL_api::initialize(const char* application_name, platform_state* plat_state) {
    if (!OpenGL_create_context()) {
        RH_FATAL("Could not create OpenGL Context!");
        return false;
    }

    OpenGL_set_swap_interval(1);

    return true;
}
void OpenGL_api::shutdown() {

}

void OpenGL_api::resized(uint16 width, uint16 height) {

}

bool32 OpenGL_api::begin_frame(real32 delta_time) {
    return true;
}
bool32 OpenGL_api::end_frame(real32 delta_time) {
    return true;
}



void OpenGL_api::create_texture(struct texture_2D* texture, const uint8* data) {

}
void OpenGL_api::destroy_texture(struct texture_2D* texture) {

}
void OpenGL_api::create_mesh(struct triangle_mesh* mesh, uint32 num_verts, const void* vertices, uint32 num_inds, const uint32* indices) {
    // to be in the triangle_mesh struct
    uint32 vao;
    uint64 vertex_size = 4*(3 + 3 + 3 + 3 + 2);

    // first create the VBO
    uint32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, num_verts*vertex_size, vertices, GL_STATIC_DRAW);

    // create EBO
    uint32 ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_inds*sizeof(uint32), indices, GL_STATIC_DRAW);

    // create the VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // assign vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
#if 0
    glEnableVertexAttribArray(n);
    glVertexAttribIPointer(n, 
                           GetComponentCount(Type), 
                           ShaderDataTypeToOpenGLBaseType(Type), 
                           Stride, 
                           (const void*)(_off));
    glVertexAttribPointer(n,
                          GetComponentCount(Type),
                          ShaderDataTypeToOpenGLBaseType(Type),
                          Normalized ? GL_TRUE : GL_FALSE,
                          Stride,
                          (const void*)(_off));
#endif

    // assign index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexArray(0);
}
void OpenGL_api::destroy_mesh(struct triangle_mesh* mesh) {
    //glDeleteVertexArrays(1, &handle);
}
void OpenGL_api::shader_create(struct shader* shader_prog, const char* shader_source) {

}
void OpenGL_api::shader_destroy(struct shader* shader_prog) {

}