#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "Engine/Core/Base.hpp"
#include "Engine/Core/MemoryArena.hpp"

/* OpenGL Commands used:
 *  Binding shader and setting uniforms:
 *  CMD_Bind_Shader
 *  CMD_Upload_Uniform_[int/float/vector/matrix] -> by name? by index?
 * 
 * prePassShader->Bind();
    prePassShader->SetMat4("r_Projection", ProjectionMatrix);
    prePassShader->SetFloat("r_AlbedoTexToggle",    1.0f);
 * 
 * 
 *  Bind and operate on framebuffers:
 *  CMD_Bind_Framebuffer
 *  CMD_Clear_Buffer
 * 
   s_Data.gBuffer->Bind();
 * s_Data.gBuffer->ClearBuffers();
 * 
 * Bind textures and submit render calls:
 *  CMD_BIND_TEXTURE
 *  
 *  CMD_SUBMIT_VAO
 *  CMD_SUBMIT_Fullscreen
    s_Data.gBuffer->BindTexture(2, 2);
 * SubmitFullscreenQuad();
 * 
 * Adjust viewport  (might not be handled by us, idk really)
    *  RenderCommand::SetViewport(0, 0, width, height);
 * */

enum render_command_type {
    render_command_type_CMD_Bind_Shader,
    render_command_type_CMD_Upload_Uniform_int,
    render_command_type_CMD_Upload_Uniform_float,
    render_command_type_CMD_Upload_Uniform_vec2,
    render_command_type_CMD_Upload_Uniform_vec3,
    render_command_type_CMD_Upload_Uniform_vec4,
    render_command_type_CMD_Upload_Uniform_mat4,

    render_command_type_CMD_Bind_Framebuffer,
    render_command_type_CMD_Clear_Buffer,

    render_command_type_CMD_Bind_Texture,
    render_command_type_CMD_Bind_VAO,

    render_command_type_CMD_Submit,
};

struct render_command_header {
    render_command_type Type;
    uint32 Size;
};

struct CMD_Bind_Shader {
};
struct CMD_Upload_Uniform_int {

};
struct CMD_Upload_Uniform_float {

};
struct CMD_Upload_Uniform_vec2 {

};
struct CMD_Upload_Uniform_vec3 {

};
struct CMD_Upload_Uniform_vec4 {

};
struct CMD_Upload_Uniform_mat4 {

};
struct CMD_Bind_Framebuffer {

};
struct CMD_Clear_Buffer {
    rh::laml::Vec4 ClearColor;
};
struct CMD_Bind_Texture {

};
struct CMD_Bind_VAO {

};
struct CMD_Submit {

};

struct render_command_buffer {
    uint32 MaxSize;
    uint32 CurrentSize;
    uint8* Base;

    uint32 ElementCount;
};

render_command_buffer* CreateRenderCommandBuffer(memory_arena* Arena, uint32 MaxBufferSize);


#define PushRenderCommand(Buffer, type) (type*)PushRenderCommand_(Buffer, sizeof(type), render_command_type_##type)
inline void* PushRenderCommand_(render_command_buffer *Buffer, uint32 Size, render_command_type Type);

#endif