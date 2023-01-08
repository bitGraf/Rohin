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

    render_command_type_CMD_Set_Cull,
    render_command_type_CMD_Set_Depth_Test,
};

struct render_command_header {
    render_command_type Type;
    uint32 Size;
};

// macro test, dont really like it >>
#define CAT_(a, b) a ## b
#define CAT(a, b) CAT_(a, b)
#define UNIQUE_VARNAME(Var) CAT(Var, __LINE__)

////////////////////////////////////////////////////////////////////////////////////////////////////
#define Render_BindShader(CmdBuffer, Shader) \
    CMD_Bind_Shader* UNIQUE_VARNAME(cmd_bind_) = PushRenderCommand(CmdBuffer, CMD_Bind_Shader); \
    UNIQUE_VARNAME(cmd_bind_)->ShaderHandle = Shader.Handle;
struct CMD_Bind_Shader {
    uint32 ShaderHandle;
};
#define Render_UploadInt(CmdBuffer, Uniform, Int) \
    CMD_Upload_Uniform_int* UNIQUE_VARNAME(cmd_upload_) = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_int); \
    UNIQUE_VARNAME(cmd_upload_)->Location = Uniform.Location;\
    UNIQUE_VARNAME(cmd_upload_)->Value = Int;
struct CMD_Upload_Uniform_int {
    uint32 Location;
    int32 Value;
};
#define Render_UploadFloat(CmdBuffer, Uniform, Float) \
    CMD_Upload_Uniform_float* UNIQUE_VARNAME(cmd_upload_) = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_float); \
    UNIQUE_VARNAME(cmd_upload_)->Location = Uniform.Location;\
    UNIQUE_VARNAME(cmd_upload_)->Value = Float;
struct CMD_Upload_Uniform_float {
    uint32 Location;
    real32 Value;
};
#define Render_UploadVec2(CmdBuffer, Uniform, Float2) \
    CMD_Upload_Uniform_vec2* UNIQUE_VARNAME(cmd_upload_) = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_vec2); \
    UNIQUE_VARNAME(cmd_upload_)->Location = Uniform.Location;\
    UNIQUE_VARNAME(cmd_upload_)->Value = Float2;
struct CMD_Upload_Uniform_vec2 {
    uint32 Location;
    rh::laml::Vec2 Value;
};
#define Render_UploadVec3(CmdBuffer, Uniform, Float3) \
    CMD_Upload_Uniform_vec3* UNIQUE_VARNAME(cmd_upload_) = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_vec3); \
    UNIQUE_VARNAME(cmd_upload_)->Location = Uniform.Location;\
    UNIQUE_VARNAME(cmd_upload_)->Value = Float3;
struct CMD_Upload_Uniform_vec3 {
    uint32 Location;
    rh::laml::Vec3 Value;
};
#define Render_UploadVec4(CmdBuffer, Uniform, Float4) \
    CMD_Upload_Uniform_vec4* UNIQUE_VARNAME(cmd_upload_) = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_vec4); \
    UNIQUE_VARNAME(cmd_upload_)->Location = Uniform.Location;\
    UNIQUE_VARNAME(cmd_upload_)->Value = Float4;
struct CMD_Upload_Uniform_vec4 {
    uint32 Location;
    rh::laml::Vec4 Value;
};
#define Render_UploadMat4(CmdBuffer, Uniform, Float4x4) \
    CMD_Upload_Uniform_mat4* UNIQUE_VARNAME(cmd_upload_) = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_mat4); \
    UNIQUE_VARNAME(cmd_upload_)->Location = Uniform.Location;\
    UNIQUE_VARNAME(cmd_upload_)->Value = Float4x4;
struct CMD_Upload_Uniform_mat4 {
    uint32 Location;
    rh::laml::Mat4 Value;
};
struct CMD_Bind_Framebuffer {

};
#define Render_ClearColor(CmdBuffer, r, g, b, a) \
    CMD_Clear_Buffer* UNIQUE_VARNAME(cmd_clear_) = PushRenderCommand(CmdBuffer, CMD_Clear_Buffer); \
    UNIQUE_VARNAME(cmd_clear_)->ClearColor.x = r;\
    UNIQUE_VARNAME(cmd_clear_)->ClearColor.y = g;\
    UNIQUE_VARNAME(cmd_clear_)->ClearColor.z = b;\
    UNIQUE_VARNAME(cmd_clear_)->ClearColor.w = a;
struct CMD_Clear_Buffer {
    rh::laml::Vec4 ClearColor;
};
#define Render_BindTexture(CmdBuffer, Slot, Handle) \
    CMD_Bind_Texture* UNIQUE_VARNAME(cmd_bind_) = PushRenderCommand(CmdBuffer, CMD_Bind_Texture); \
    UNIQUE_VARNAME(cmd_bind_)->TextureSlot = Slot;\
    UNIQUE_VARNAME(cmd_bind_)->TextureHandle = Handle;
struct CMD_Bind_Texture {
    uint32 TextureSlot;
    uint32 TextureHandle;
};
#define Render_BindVAO(CmdBuffer, VAO) \
    CMD_Bind_VAO* UNIQUE_VARNAME(cmd_bind_) = PushRenderCommand(CmdBuffer, CMD_Bind_VAO); \
    UNIQUE_VARNAME(cmd_bind_)->VAOHandle = VAO.Handle;
struct CMD_Bind_VAO {
    uint32 VAOHandle;
};
#define Render_Submit(CmdBuffer, Count) \
    CMD_Submit* UNIQUE_VARNAME(cmd_submit_) = PushRenderCommand(CmdBuffer, CMD_Submit); \
    UNIQUE_VARNAME(cmd_submit_)->IndexCount = Count;
struct CMD_Submit {
    uint32 IndexCount;
};
#define Render_SetFrontCull(CmdBuffer, Value) \
    CMD_Set_Cull* UNIQUE_VARNAME(cmd_set_) = PushRenderCommand(CmdBuffer, CMD_Set_Cull); \
    UNIQUE_VARNAME(cmd_set_)->Front = Value;
struct CMD_Set_Cull {
    bool32 Front;
};
#define Render_SetDepthTest(CmdBuffer, Value) \
    CMD_Set_Depth_Test* UNIQUE_VARNAME(cmd_set_) = PushRenderCommand(CmdBuffer, CMD_Set_Depth_Test); \
    UNIQUE_VARNAME(cmd_set_)->Enabled = Value;
struct CMD_Set_Depth_Test {
    bool32 Enabled;
};
//////////////////////////////////////////////////////////////////////////////////////////////////


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