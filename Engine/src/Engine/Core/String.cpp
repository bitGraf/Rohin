#include "String.h"

#include "Engine/Core/Asserts.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Memory/Memory.h"

// todo: STD library
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

uint8* AdvanceBufferSize_(uint8** Buffer, uint64 Size, uint8* End) {
    AssertMsg((*Buffer + Size) <= End, "Reached the end of the buffer!");
    uint8* Result = *Buffer;
    *Buffer += Size;
    return Result;
}

int string_build(char* buffer, int buf_size, char* fmt, ...) {
    // todo: STD library
    va_list args;
    va_start(args, fmt);

    int res = vsnprintf(buffer, buf_size, fmt, args);

    va_end(args);

    return res;
}

int string_compare(const char* str1, const char* str2) {
    // todo: STD library
    return strcmp(str1, str2);
}

char* copy_string_to_arena(const char* str, memory_arena* arena) {
    size_t len = strlen(str);
    char* new_str = PushArray(arena, char, len + 1);
    memory_copy(new_str, str, len);
    new_str[len] = 0;
    return new_str;
}