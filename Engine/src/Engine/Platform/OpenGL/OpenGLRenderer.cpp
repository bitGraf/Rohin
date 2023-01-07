#include "OpenGLRenderer.h"

#include <stdarg.h>

internal_func GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType Type) {
    switch (Type) {
        case ShaderDataType::Float:  return GL_FLOAT;
        case ShaderDataType::Float2: return GL_FLOAT;
        case ShaderDataType::Float3: return GL_FLOAT;
        case ShaderDataType::Float4: return GL_FLOAT;
        case ShaderDataType::Int:    return GL_INT;
        case ShaderDataType::Int2:   return GL_INT;
        case ShaderDataType::Int3:   return GL_INT;
        case ShaderDataType::Int4:   return GL_INT;
        case ShaderDataType::Mat3:   return GL_FLOAT;
        case ShaderDataType::Mat4:   return GL_FLOAT;
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
        case ShaderDataType::Float:  return 1;      break;
        case ShaderDataType::Float2: return 2;      break;
        case ShaderDataType::Float3: return 3;      break;
        case ShaderDataType::Float4: return 4;      break;
        case ShaderDataType::Int:    return 1;      break;
        case ShaderDataType::Int2:   return 2;      break;
        case ShaderDataType::Int3:   return 3;      break;
        case ShaderDataType::Int4:   return 4;      break;
        case ShaderDataType::Mat3:   return 3 * 3;  break;
        case ShaderDataType::Mat4:   return 4 * 4;  break;
        case ShaderDataType::Bool:   return 1;      break;
    }

    Assert(false);
    return 0;
}

internal_func uint32 ShaderDataTypeSize(ShaderDataType Type) {
    switch (Type) {
        case ShaderDataType::Float:  return 4;          break;
        case ShaderDataType::Float2: return 4 * 2;      break;
        case ShaderDataType::Float3: return 4 * 3;      break;
        case ShaderDataType::Float4: return 4 * 4;      break;
        case ShaderDataType::Int:    return 4;          break;
        case ShaderDataType::Int2:   return 4 * 2;      break;
        case ShaderDataType::Int3:   return 4 * 3;      break;
        case ShaderDataType::Int4:   return 4 * 4;      break;
        case ShaderDataType::Mat3:   return 4 * 3 * 3; break;
        case ShaderDataType::Mat4:   return 4 * 4 * 4; break;
        case ShaderDataType::Bool:   return 4;         break;
    }

    Assert(false);
    return 0;
}

internal_func vertex_buffer OpenGLCreateVertexBuffer(void* VertexData, uint32 DataSizeInBytes, uint8 NumAttributes, ShaderDataType First, ...) {
    vertex_buffer Result = {};

    glCreateBuffers(1, &Result.Handle);
    glBindBuffer(GL_ARRAY_BUFFER, Result.Handle);
    glBufferData(GL_ARRAY_BUFFER, DataSizeInBytes, VertexData, GL_STATIC_DRAW);

    va_list Next;

    Result.NumAttributes = NumAttributes;
    Result.Attributes[0] = First;

    if (NumAttributes > 1) {
        va_start(Next, First);

        for (int n = 1; n < NumAttributes; n++) {
            Result.Attributes[n] = va_arg(Next, ShaderDataType);
        }

        va_end(Next);
    }

    return Result;
}

internal_func index_buffer OpenGLCreateIndexBuffer(uint32* IndexData, uint32 IndexCount) {
    index_buffer Result = {};

    glCreateBuffers(1, &Result.Handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Result.Handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount*sizeof(uint32), IndexData, GL_STATIC_DRAW);

    Result.IndexCount = IndexCount;

    return Result;
}

internal_func vertex_array_object OpenGLCreateVertexArray(vertex_buffer* VBO, index_buffer* IBO) {
    vertex_array_object Result = {};

    bool32 Normalized = false;

    // Calculate offsets and strides of attributes
    uint32 Stride = 0;
    uint32 Offsets[MAX_ATTRIBUTES] = {0};
    uint32 tmp_offset = 0;
    for (int n = 0; n < VBO->NumAttributes; n++) {
        ShaderDataType Type = VBO->Attributes[n];

        Offsets[n] = tmp_offset;
        tmp_offset += ShaderDataTypeSize(Type);
        Stride += ShaderDataTypeSize(Type);
    }

    // Create VAO
    glCreateVertexArrays(1, &Result.Handle);
    glBindVertexArray(Result.Handle);

    // Add Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO->Handle);
    Assert(VBO->NumAttributes);
    for (int n = 0; n < VBO->NumAttributes; n++) {
        ShaderDataType Type = VBO->Attributes[n];
        size_t _off = Offsets[n];

        glEnableVertexAttribArray(n);
        if (IsIntegerType(Type)) {
            glVertexAttribIPointer(n, 
                                   GetComponentCount(Type), 
                                   ShaderDataTypeToOpenGLBaseType(Type), 
                                   Stride, 
                                   (const void*)(_off));
        } else {
            glVertexAttribPointer(n,
                                  GetComponentCount(Type),
                                  ShaderDataTypeToOpenGLBaseType(Type),
                                  Normalized ? GL_TRUE : GL_FALSE,
                                  Stride,
                                  (const void*)(_off));
        }
    }

    // Add Index Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO->Handle);
    Result.IndexCount = IBO->IndexCount;

    // unbind
    glBindVertexArray(0);

    return Result;
}


internal_func uint32 OpenGLCreateTexture(uint8* Bitmap, uint32 Resolution) {
    uint32 TextureHandle;

    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Resolution, Resolution, 0, GL_RED, GL_UNSIGNED_BYTE, Bitmap);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    /*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    */

    return TextureHandle;
}

#define CheckToken(Start, Token) CheckToken_(Start, Token, sizeof(Token))
internal_func bool32 
CheckToken_(uint8* Start, const char Token[], uint8 TokenLength) {
    for (uint8 TokenIndex = 0; TokenIndex < TokenLength; TokenIndex++) {
        if (Start[TokenIndex] != Token[TokenIndex]) {
            return false;
        }
    }

    return true;
}

#define FindAfterToken(Buffer, BufferLength, Token) FindAfterToken_(Buffer, BufferLength, Token, sizeof(Token))
internal_func uint8* 
FindAfterToken_(uint8* Buffer, uint32 BufferLength, const char Token[], uint8 TokenLength) {
    uint8* Result = nullptr;

    for (uint8* Scan = Buffer; *Scan; Scan++) {
        if (*Scan == Token[0]) {
            if (CheckToken_(Scan, Token, TokenLength-1)) {
                Result = Scan + TokenLength;
                break;
            }
        }
    }

    return Result;
}

internal_func bool32 
OpenGLLoadShader(Shader* shader, uint8* Buffer, uint32 BytesRead) {
    uint8* VertexStart = Buffer;
    VertexStart = FindAfterToken(Buffer, BytesRead, "#type vertex");
    Assert(VertexStart);
    VertexStart++;

    uint8* FragmentStart = Buffer;
    FragmentStart = FindAfterToken(Buffer, BytesRead, "#type fragment");
    Assert(FragmentStart);
    FragmentStart++;

    uint32 VertexLength   = (uint32)((FragmentStart - VertexStart) - sizeof("#type fragment") - 1);
    uint32 FragmentLength = (uint32)(BytesRead - (FragmentStart - Buffer) + 1);

    Assert(VertexLength && VertexLength < BytesRead);
    Assert(FragmentLength && FragmentLength < BytesRead);

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

        OutputDebugStringA("Vertex shader failed to compile: \n");
        OutputDebugStringA(ErrorBuffer);

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

        OutputDebugStringA("Fragment shader failed to compile: \n");
        OutputDebugStringA(ErrorBuffer);

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

        OutputDebugStringA("Program failed to link: \n");
        OutputDebugStringA(ErrorBuffer);

        return false;
    }

    glDetachShader(programID, vertShaderID);
    glDetachShader(programID, fragShaderID);

    // Assign the values in the Shader wrapper
    shader->Handle = programID;

    return true;
}

internal_func void 
OpenGLBeginFrame() {
}

internal_func void 
OpenGLEndFrame(win32_window_dimension* Dimension, render_command_buffer* CommandBuffer) {
    glViewport(0, 0, Dimension->Width, Dimension->Height);

    uint32 CommandCount = CommandBuffer->ElementCount;
    uint32 offset = 0;
    for (uint32 CommandIndex = 0; CommandIndex < CommandCount; ++CommandIndex) {
        render_command_header* Header = (render_command_header*)(CommandBuffer->Base + offset);
        offset += Header->Size;

        void* Data = (uint8*)Header + sizeof(*Header);
        switch(Header->Type) {
            case render_command_type_CMD_Bind_Shader: {
                CMD_Bind_Shader* cmd = (CMD_Bind_Shader*)Data;

                glUseProgram(cmd->ShaderHandle);
            } break;
    
            case render_command_type_CMD_Upload_Uniform_int: {
                CMD_Upload_Uniform_int* cmd = (CMD_Upload_Uniform_int*)Data;

                glUniform1i(cmd->Location, cmd->Value);
            } break;
    
            case render_command_type_CMD_Upload_Uniform_float: {
                CMD_Upload_Uniform_float* cmd = (CMD_Upload_Uniform_float*)Data;

                glUniform1f(cmd->Location, cmd->Value);
            } break;
    
            case render_command_type_CMD_Upload_Uniform_vec2: {
                CMD_Upload_Uniform_vec2* cmd = (CMD_Upload_Uniform_vec2*)Data;

                glUniform2f(cmd->Location, cmd->Value.x, cmd->Value.y);
            } break;
    
            case render_command_type_CMD_Upload_Uniform_vec3: {
                CMD_Upload_Uniform_vec3* cmd = (CMD_Upload_Uniform_vec3*)Data;

                glUniform3f(cmd->Location, cmd->Value.x, cmd->Value.y, cmd->Value.z);
            } break;
    
            case render_command_type_CMD_Upload_Uniform_vec4: {
                CMD_Upload_Uniform_vec4* cmd = (CMD_Upload_Uniform_vec4*)Data;

                glUniform4f(cmd->Location, cmd->Value.x, cmd->Value.y, cmd->Value.z, cmd->Value.w);
            } break;
    
            case render_command_type_CMD_Upload_Uniform_mat4: {
                CMD_Upload_Uniform_mat4* cmd = (CMD_Upload_Uniform_mat4*)Data;

                glUniformMatrix4fv(cmd->Location, 1, GL_FALSE, &cmd->Value.c_11);
            } break;
    
            case render_command_type_CMD_Bind_Framebuffer: {
                CMD_Bind_Framebuffer* cmd = (CMD_Bind_Framebuffer*)Data;
            } break;
    
            case render_command_type_CMD_Clear_Buffer: {
                CMD_Clear_Buffer* cmd = (CMD_Clear_Buffer*)Data;

                glClearColor(cmd->ClearColor.x, cmd->ClearColor.y, cmd->ClearColor.z, cmd->ClearColor.w);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            } break;
    
            case render_command_type_CMD_Bind_Texture: {
                CMD_Bind_Texture* cmd = (CMD_Bind_Texture*)Data;

                glActiveTexture(GL_TEXTURE0 + cmd->TextureSlot);
                glBindTexture(GL_TEXTURE_2D, cmd->TextureHandle);
            } break;
    
            case render_command_type_CMD_Bind_VAO: {
                CMD_Bind_VAO* cmd = (CMD_Bind_VAO*)Data;

                glBindVertexArray(cmd->VAOHandle);
            } break;
    
            case render_command_type_CMD_Submit: { 
                CMD_Submit* cmd = (CMD_Submit*)Data;

                glDrawElements(GL_TRIANGLES, cmd->IndexCount, GL_UNSIGNED_INT, nullptr);
            } break;

            case render_command_type_CMD_Set_Cull: {
                CMD_Set_Cull* cmd = (CMD_Set_Cull*)Data;

                glCullFace(cmd->Front ? GL_FRONT : GL_BACK);
            } break;
            case render_command_type_CMD_Set_Depth_Test: {
                CMD_Set_Depth_Test* cmd = (CMD_Set_Depth_Test*)Data;

                cmd->Enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
            } break;

            default: {
                Assert(false);
            } break;
        }
    }

    // Clear the command buffer
    CommandBuffer->CurrentSize = 0;
    CommandBuffer->ElementCount = 0;
}
