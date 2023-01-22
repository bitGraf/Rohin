#pragma once

#include "Engine/Defines.h"

// buffer reading utils
#define AdvanceBufferArray(Buffer, Type, Count, End) (Type*)AdvanceBufferSize_(Buffer, (Count)*sizeof(Type), End)
#define AdvanceBuffer(Buffer, Type, End) (Type*)AdvanceBufferSize_(Buffer, sizeof(Type), End)
uint8* AdvanceBufferSize_(uint8** Buffer, uint32 Size, uint8* End);