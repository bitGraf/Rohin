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
    glGenTextures(1, &texture->handle);
    glBindTexture(GL_TEXTURE_2D, texture->handle);

    GLenum InternalFormat = 0;
    GLenum Format = 0;
    switch (texture->num_channels) {
        case 1: {
            InternalFormat = GL_R8;
            Format = GL_RED;
        } break;
        case 2: {
            InternalFormat = GL_RG8;
            Format = GL_RG;
        } break;
        case 3: {
            InternalFormat = GL_RGB8;
            Format = GL_RGB;
        } break;
        case 4: {
            InternalFormat = GL_RGBA8;
            Format = GL_RGBA;
        } break;
        default:
            Assert(false);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, texture->width, texture->height, 0, Format, GL_UNSIGNED_BYTE, data);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ResolutionX, ResolutionY, 0, GL_RED, GL_UNSIGNED_BYTE, Bitmap);
    glGenerateMipmap(GL_TEXTURE_2D);

    // This for font texture
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    // This for everything else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void OpenGL_api::destroy_texture(struct texture_2D* texture) {
    glDeleteTextures(1, &texture->handle);
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

// TODO: move these string-parsing functions to a better place!
#define CheckToken(Start, Token) CheckToken_(Start, Token, sizeof(Token))
internal_func bool32 
    CheckToken_(const uint8* Start, const char Token[], uint8 TokenLength) {
    for (uint8 TokenIndex = 0; TokenIndex < TokenLength; TokenIndex++) {
        if (Start[TokenIndex] != Token[TokenIndex]) {
            return false;
        }
    }

    return true;
}

#define FindAfterToken(Buffer, BufferLength, Token) FindAfterToken_(Buffer, BufferLength, Token, sizeof(Token))
internal_func const uint8* 
    FindAfterToken_(const uint8* Buffer, uint64 BufferLength, const char Token[], uint8 TokenLength) {
    const uint8* Result = nullptr;

    for (const uint8* Scan = Buffer; *Scan; Scan++) {
        if (*Scan == Token[0]) {
            if (CheckToken_(Scan, Token, TokenLength-1)) {
                Result = Scan + TokenLength;
                break;
            }
        }
    }

    return Result;
}

bool32 OpenGL_api::create_shader(shader* shader_prog, const uint8* shader_source, uint64 num_bytes) {
    const uint8* VertexStart = shader_source;
    VertexStart = FindAfterToken(shader_source, num_bytes, "#type vertex");
    Assert(VertexStart);
    VertexStart++;

    const uint8* FragmentStart = shader_source;
    FragmentStart = FindAfterToken(shader_source, num_bytes, "#type fragment");
    Assert(FragmentStart);
    FragmentStart++;

    uint32 VertexLength   = (uint32)((FragmentStart - VertexStart) - sizeof("#type fragment") - 1);
    uint32 FragmentLength = (uint32)(num_bytes - (FragmentStart - shader_source) + 1);

    Assert(VertexLength && VertexLength < num_bytes);
    Assert(FragmentLength && FragmentLength < num_bytes);

    GLuint programID = glCreateProgram();

    // Compile the vertex shader
    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShaderID, 1, (const GLchar**)(&VertexStart), (GLint*)(&VertexLength));
    glCompileShader(vertShaderID);

    GLint isCompiled = 0;
    glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &maxLength);

        //std::vector<GLchar> infoLog(maxLength);
        char ErrorBuffer[256];
        GLint actLength;
        glGetShaderInfoLog(vertShaderID, 256, &actLength, ErrorBuffer);

        glDeleteShader(vertShaderID);

        RH_ERROR("Vertex shader failed to compile:\n            %s", ErrorBuffer);

        return false;
    }
    glAttachShader(programID, vertShaderID);

    // Compile the fragment shader
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderID, 1, (const GLchar**)(&FragmentStart), (GLint*)(&FragmentLength));
    glCompileShader(fragShaderID);

    glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &maxLength);

        //std::vector<GLchar> infoLog(maxLength);
        char ErrorBuffer[256];
        GLint actLength;
        glGetShaderInfoLog(fragShaderID, 256, &actLength, ErrorBuffer);

        glDeleteShader(fragShaderID);

        RH_ERROR("Fragment shader failed to compile:\n            %s", ErrorBuffer);

        return false;
    }
    glAttachShader(programID, fragShaderID);

    // Link the two shaders into the program
    glLinkProgram(programID);
    GLint isLinked = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

        //std::vector<GLchar> infoLog(maxLength);
        char ErrorBuffer[256];
        GLint actLength;
        glGetProgramInfoLog(programID, 256, &actLength, ErrorBuffer);

        glDeleteProgram(programID);
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);

        RH_ERROR("Program failed to link:\n            %s", ErrorBuffer);

        return false;
    }

    glDetachShader(programID, vertShaderID);
    glDetachShader(programID, fragShaderID);

    // Assign the values in the Shader wrapper
    shader_prog->handle = programID;

    return true;
}
void OpenGL_api::destroy_shader(shader* shader_prog) {
    glDeleteShader(shader_prog->handle);
    shader_prog->handle = 0;
}