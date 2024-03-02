#include "Engine/Defines.h"

#include "imgui_widgets.cpp"
#include "imgui.cpp"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_tables.cpp"

#ifdef RH_PLATFORM_WINDOWS
    #include "backends/imgui_impl_win32.cpp"
#endif

#pragma warning ( push )
#pragma warning ( disable: 4701 )

// Line 940: warning C4701: potentially uninitialized local variable 'last_pixel_unpack_buffer' used
//           This one is fine (as of 3/2/2024) v1.90.4
#include "backends/imgui_impl_opengl3.cpp"

#pragma warning ( pop ) 