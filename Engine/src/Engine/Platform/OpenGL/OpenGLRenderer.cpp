#include "OpenGLRenderer.h"

internal_func void OpenGLBeginFrame(win32_window_dimension* Dimension) {
    // TODO: Wrong place for this >.>
    glViewport(0, 0, Dimension->Width, Dimension->Height);
}

internal_func void OpenGLEndFrame(render_command_buffer* CommandBuffer) {
    uint32 CommandCount = CommandBuffer->ElementCount;
    uint32 offset = 0;
    for (uint32 CommandIndex = 0; CommandIndex < CommandCount; ++CommandIndex) {
        render_command_header* Header = (render_command_header*)(CommandBuffer->Base + offset);
        offset += Header->Size;

        void* Data = (uint8*)Header + sizeof(*Header);
        switch(Header->Type) {
            case render_command_type_CMD_Bind_Shader: {
                CMD_Bind_Shader* cmd = (CMD_Bind_Shader*)Data;
            } break;
    
            case render_command_type_CMD_Upload_Uniform_int: {
                CMD_Upload_Uniform_int* cmd = (CMD_Upload_Uniform_int*)Data;
            } break;
    
            case render_command_type_CMD_Upload_Uniform_float: {
                CMD_Upload_Uniform_float* cmd = (CMD_Upload_Uniform_float*)Data;
            } break;
    
            case render_command_type_CMD_Upload_Uniform_vec2: {
                CMD_Upload_Uniform_vec2* cmd = (CMD_Upload_Uniform_vec2*)Data;
            } break;
    
            case render_command_type_CMD_Upload_Uniform_vec3: {
                CMD_Upload_Uniform_vec3* cmd = (CMD_Upload_Uniform_vec3*)Data;
            } break;
    
            case render_command_type_CMD_Upload_Uniform_vec4: {
                CMD_Upload_Uniform_vec4* cmd = (CMD_Upload_Uniform_vec4*)Data;
            } break;
    
            case render_command_type_CMD_Upload_Uniform_mat4: {
                CMD_Upload_Uniform_mat4* cmd = (CMD_Upload_Uniform_mat4*)Data;
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
            } break;
    
            case render_command_type_CMD_Bind_VAO: {
                CMD_Bind_VAO* cmd = (CMD_Bind_VAO*)Data;
            } break;
    
            case render_command_type_CMD_Submit: { 
                CMD_Submit* cmd = (CMD_Submit*)Data; 
            } break;

        }
    }
}
