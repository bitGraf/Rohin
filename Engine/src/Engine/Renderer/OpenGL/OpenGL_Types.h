#pragma once

#include "Engine/Defines.h"


#if RH_PLATFORM_WINDOWS

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

#if defined(RH_COMPILE_OPENGL_4_4)
    #include "Engine/Platform/GLAD_4_4/include/glad/glad.h"
#elif defined(RH_COMPILE_OPENGL_4_3)
    #include "Engine/Platform/GLAD_4_3/include/glad/glad.h"
#else
    #error "No version of OpenGL defined"
#endif