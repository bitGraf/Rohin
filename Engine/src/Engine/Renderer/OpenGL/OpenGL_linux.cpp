#include "OpenGL_Platform.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"

#if RH_PLATFORM_LINUX

bool32 OpenGL_create_context() { 
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return false;
}
//void OpenGL_swap_buffers();

void OpenGL_set_swap_interval(int32 interval) {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
}
int32 OpenGL_get_swap_interval() { 
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return 0;
}

#endif