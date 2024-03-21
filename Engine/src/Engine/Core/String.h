#pragma once

#include "Engine/Defines.h"

struct memory_arena;

// buffer reading utils
#define AdvanceBufferArray(Buffer, Type, Count, End) (Type*)AdvanceBufferSize_(Buffer, (Count)*sizeof(Type), End)
#define AdvanceBuffer(Buffer, Type, End) (Type*)AdvanceBufferSize_(Buffer, sizeof(Type), End)
uint8* AdvanceBufferSize_(uint8** Buffer, uint64 Size, uint8* End);

RHAPI int string_build(char* buffer, int buf_size, char* fmt, ...);
RHAPI int string_compare(const char* str1, const char* str2);
RHAPI char* copy_string_to_arena(const char* str, memory_arena* arena);