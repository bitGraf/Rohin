#pragma once

#include "Engine/Defines.h"

RHAPI void* memory_zero(void* memory, uint64 size);
RHAPI void* memory_copy(void* dest, const void* src, uint64 size);
RHAPI void* memory_set(void* memory, uint8 value, uint64 size);