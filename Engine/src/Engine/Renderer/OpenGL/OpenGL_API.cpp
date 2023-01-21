#include "OpenGL_API.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"

#include "OpenGL_Types.h"
#include "OpenGL_Platform.h"

#include <stdarg.h>

internal_func GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType Type) {
    switch (Type) {
        case ShaderDataType::Float:  return GL_FLOAT;
        case ShaderDataType::Float2: return GL_FLOAT;
        case ShaderDataType::Float3: return GL_FLOAT;
        case ShaderDataType::Float4: return GL_FLOAT;
        case ShaderDataType::Mat3:   return GL_FLOAT;
        case ShaderDataType::Mat4:   return GL_FLOAT;

        case ShaderDataType::Int:    return GL_INT;
        case ShaderDataType::Int2:   return GL_INT;
        case ShaderDataType::Int3:   return GL_INT;
        case ShaderDataType::Int4:   return GL_INT;

        case ShaderDataType::Bool:   return GL_BOOL;
    }

    Assert(false);
    return 0;
}

internal_func bool IsIntegerType(ShaderDataType Type) {
    switch (Type) {
        case ShaderDataType::Int:    return true;
        case ShaderDataType::Int2:   return true;
        case ShaderDataType::Int3:   return true;
        case ShaderDataType::Int4:   return true;

        case ShaderDataType::Float:  return false;
        case ShaderDataType::Float2: return false;
        case ShaderDataType::Float3: return false;
        case ShaderDataType::Float4: return false;
        case ShaderDataType::Mat3:   return false;
        case ShaderDataType::Mat4:   return false;
        case ShaderDataType::Bool:   return false;
    }

    Assert(false);
    return 0;
}

internal_func uint32 GetComponentCount(ShaderDataType Type) {
    switch (Type) {
        case ShaderDataType::Float:  return 1;
        case ShaderDataType::Int:    return 1;
        case ShaderDataType::Bool:   return 1;

        case ShaderDataType::Float2: return 2;
        case ShaderDataType::Int2:   return 2;

        case ShaderDataType::Float3: return 3;
        case ShaderDataType::Int3:   return 3;

        case ShaderDataType::Float4: return 4;
        case ShaderDataType::Int4:   return 4;

        case ShaderDataType::Mat3:   return 3 * 3;
        case ShaderDataType::Mat4:   return 4 * 4;
    }

    Assert(false);
    return 0;
}

internal_func uint32 ShaderDataTypeSize(ShaderDataType Type) {
    switch (Type) {
        case ShaderDataType::Float:  return 4;
        case ShaderDataType::Int:    return 4;
        case ShaderDataType::Bool:   return 4;

        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Int2:   return 4 * 2;

        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Int3:   return 4 * 3;

        case ShaderDataType::Float4: return 4 * 4;
        case ShaderDataType::Int4:   return 4 * 4;

        case ShaderDataType::Mat3:   return 4 * 3 * 3;
        case ShaderDataType::Mat4:   return 4 * 4 * 4;
    }

    Assert(false);
    return 0;
}

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

#define MAX_VERTEX_ATTRIBUTES 16

struct vertex_layout {
    uint32 stride;
    uint32 num_attributes;
};

vertex_layout calculate_stride(const ShaderDataType* attributes) {
    vertex_layout layout = {};
    for (const ShaderDataType* scan = attributes; *scan != ShaderDataType::None; scan++) {
        layout.num_attributes++;
        Assert(layout.num_attributes < MAX_VERTEX_ATTRIBUTES); // just limit it to a reasonable amount now
        layout.stride += ShaderDataTypeSize(*scan);
    }
    Assert(layout.num_attributes > 0);
    return layout;
}


void OpenGL_api::create_mesh(triangle_geometry* mesh, 
                             uint32 num_verts, const void* vertices, 
                             uint32 num_inds, const uint32* indices,
                             const ShaderDataType* attributes) {
    // to be in the triangle_mesh struct
    vertex_layout layout = calculate_stride(attributes);
    bool32 normalized = false;

    mesh->num_verts = num_verts;
    mesh->num_inds = num_inds;
    mesh->flag = 0;

    // first create the VBO
    uint32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, num_verts*layout.stride, vertices, GL_STATIC_DRAW);

    // create EBO
    uint32 ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_inds*sizeof(uint32), indices, GL_STATIC_DRAW);

    // create the VAO
    glGenVertexArrays(1, &mesh->handle);
    glBindVertexArray(mesh->handle);

    // assign vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    size_t offset = 0;
    for (uint32 n = 0; n < layout.num_attributes; n++) {
        ShaderDataType type = attributes[n];

        glEnableVertexAttribArray(n);

        if (IsIntegerType(type)) {
            glVertexAttribIPointer(n, 
                                   GetComponentCount(type), 
                                   ShaderDataTypeToOpenGLBaseType(type), 
                                   layout.stride, 
                                   (const void*)(offset));
        } else {
            glVertexAttribPointer(n,
                                  GetComponentCount(type),
                                  ShaderDataTypeToOpenGLBaseType(type),
                                  normalized ? GL_TRUE : GL_FALSE,
                                  layout.stride,
                                  (const void*)(offset));
        }

        offset += ShaderDataTypeSize(type);
    }

    // assign index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexArray(0);
}
void OpenGL_api::destroy_mesh(triangle_geometry* mesh) {
    glDeleteVertexArrays(1, &mesh->handle);
}
void OpenGL_api::create_shader(struct shader* shader_prog, const char* shader_source) {

}
void OpenGL_api::destroy_shader(struct shader* shader_prog) {

}