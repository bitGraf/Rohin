#include "String.h"

#include "Engine/Core/Asserts.h"

uint8* AdvanceBufferSize_(uint8** Buffer, uint32 Size, uint8* End) {
    Assert((*Buffer + Size) <= End);
    uint8* Result = *Buffer;
    *Buffer += Size;
    return Result;
}