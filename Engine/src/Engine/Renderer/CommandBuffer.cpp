#include "CommandBuffer.h"

render_command_buffer* CreateRenderCommandBuffer(memory_arena* Arena, uint32 MaxBufferSize) {
    render_command_buffer* Result = PushStruct(Arena, render_command_buffer);

    Result->Base = (uint8*)PushSize_(Arena, MaxBufferSize);
    Result->MaxSize = MaxBufferSize;

    return Result;
}

inline void* PushRenderCommand_(render_command_buffer *Buffer, uint32 Size, render_command_type Type) {
    void* Result = 0;

    Size += sizeof(render_command_header);

    render_command_header* Header = (render_command_header*)(Buffer->Base + Buffer->CurrentSize);
    Header->Type = Type;
    Header->Size = Size;
    Result = (uint8*)Header + sizeof(*Header);

    Buffer->CurrentSize += Size;
    ++Buffer->ElementCount;

    return Result;
}