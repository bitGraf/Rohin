#include "String.h"

#include "Engine/Core/Asserts.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

uint8* AdvanceBufferSize_(uint8** Buffer, uint64 Size, uint8* End) {
    Assert((*Buffer + Size) <= End);
    uint8* Result = *Buffer;
    *Buffer += Size;
    return Result;
}

int string_build(char* buffer, int buf_size, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int res = vsnprintf(buffer, buf_size, fmt, args);

    va_end(args);

    return res;
}

int string_compare(const char* str1, const char* str2) {
    return strcmp(str1, str2);
}