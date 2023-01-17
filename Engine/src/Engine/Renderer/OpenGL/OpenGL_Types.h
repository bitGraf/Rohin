#pragma once

#include "Engine/Defines.h"


#if RH_PLATFORM_WINDOWS

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

#include "Engine/Platform/GLAD_4_3/include/glad/glad.h"