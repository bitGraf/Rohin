#include <gl/gl.h>

/******************************************************************************
 * WGL_ARB_extensions_string 
 * https://registry.khronos.org/OpenGL/extensions/ARB/WGL_ARB_extensions_string.txt
 */

typedef const char *(__stdcall * _wglGetExtensionsStringARB_PROC)(HDC hdc);

/******************************************************************************
 * WGL_ARB_create_context 
 * https://registry.khronos.org/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
 * */

typedef HGLRC (__stdcall * _wglCreateContextAttribsARB_PROC)(HDC hDC, HGLRC hShareContext, const int *attribList);

// Accepted as an attribute name in <*attribList>:
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

// Accepted as bits in the attribute value for WGL_CONTEXT_FLAGS in <*attribList>:
#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

// Accepted as bits in the attribute value for WGL_CONTEXT_PROFILE_MASK_ARB in <*attribList>:
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

// New errors returned by GetLastError:
#define ERROR_INVALID_VERSION_ARB               0x2095
#define ERROR_INVALID_PROFILE_ARB               0x2096

/******************************************************************************
 * WGL_ARB_pixel_format
 * https://registry.khronos.org/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
 * */

typedef BOOL (__stdcall * _wglGetPixelFormatAttribivARB_PROC)(HDC hdc,int iPixelFormat,int iLayerPlane,UINT nAttributes,const int *piAttributes,int *piValues);
typedef BOOL (__stdcall * _wglGetPixelFormatAttribfvARB_PROC)(HDC hdc,int iPixelFormat,int iLayerPlane,UINT nAttributes,const int *piAttributes,FLOAT *pfValues);
typedef BOOL (__stdcall * _wglChoosePixelFormatARB_PROC)(HDC hdc,const int *piAttribIList,const FLOAT *pfAttribFList,UINT nMaxFormats,int *piFormats,UINT *nNumFormats);

// Accepted in the <piAttributes> parameter array of 
// wglGetPixelFormatAttribivARB, and wglGetPixelFormatAttribfvARB, 
// and as a type in the <piAttribIList> and <pfAttribFList> 
// parameter arrays of wglChoosePixelFormatARB:
#define WGL_NUMBER_PIXEL_FORMATS_ARB            0x2000
#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_DRAW_TO_BITMAP_ARB                  0x2002
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_NEED_PALETTE_ARB                    0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB             0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB              0x2006
#define WGL_SWAP_METHOD_ARB                     0x2007
#define WGL_NUMBER_OVERLAYS_ARB                 0x2008
#define WGL_NUMBER_UNDERLAYS_ARB                0x2009
#define WGL_TRANSPARENT_ARB                     0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB           0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB         0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB          0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB         0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB         0x203B
#define WGL_SHARE_DEPTH_ARB                     0x200C
#define WGL_SHARE_STENCIL_ARB                   0x200D
#define WGL_SHARE_ACCUM_ARB                     0x200E
#define WGL_SUPPORT_GDI_ARB                     0x200F
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_STEREO_ARB                          0x2012
#define WGL_PIXEL_TYPE_ARB                      0x2013
#define WGL_COLOR_BITS_ARB                      0x2014
#define WGL_RED_BITS_ARB                        0x2015
#define WGL_RED_SHIFT_ARB                       0x2016
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_GREEN_SHIFT_ARB                     0x2018
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_BLUE_SHIFT_ARB                      0x201A
#define WGL_ALPHA_BITS_ARB                      0x201B
#define WGL_ALPHA_SHIFT_ARB                     0x201C
#define WGL_ACCUM_BITS_ARB                      0x201D
#define WGL_ACCUM_RED_BITS_ARB                  0x201E
#define WGL_ACCUM_GREEN_BITS_ARB                0x201F
#define WGL_ACCUM_BLUE_BITS_ARB                 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB                0x2021
#define WGL_DEPTH_BITS_ARB                      0x2022
#define WGL_STENCIL_BITS_ARB                    0x2023
#define WGL_AUX_BUFFERS_ARB                     0x2024

// Accepted as a value in the <piAttribIList> and <pfAttribFList>
// parameter arrays of wglChoosePixelFormatARB, and returned in the
// <piValues> parameter array of wglGetPixelFormatAttribivARB, and the
// <pfValues> parameter array of wglGetPixelFormatAttribfvARB:
#define WGL_NO_ACCELERATION_ARB                 0x2025
#define WGL_GENERIC_ACCELERATION_ARB            0x2026
#define WGL_FULL_ACCELERATION_ARB               0x2027
#define WGL_SWAP_EXCHANGE_ARB                   0x2028
#define WGL_SWAP_COPY_ARB                       0x2029
#define WGL_SWAP_UNDEFINED_ARB                  0x202A
#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_TYPE_COLORINDEX_ARB                 0x202C

static void Win32InitOpenGL(HWND Window);

/******************************************************************************
 * WGL_EXT_swap_control
 * https://registry.khronos.org/OpenGL/extensions/EXT/WGL_EXT_swap_control.txt
 * */
typedef BOOL (__stdcall * _wglSwapIntervalEXT_PROC)(int interval);
typedef int (__stdcall * _wglGetSwapIntervalEXT_PROC)(void);

/******************************************************************************
 * random funcs
 * */
#define GL_NUM_EXTENSIONS 0x821D
typedef const GLubyte * (__stdcall * _glGetStringi_PROC)(GLenum name,GLuint index);












/*  WGL Extensions on my machine:
    * WGL_ARB_buffer_region 
    * WGL_ARB_create_context 
    * WGL_ARB_create_context_no_error 
    * WGL_ARB_create_context_profile 
    * WGL_ARB_create_context_robustness 
    * WGL_ARB_context_flush_control 
    * WGL_ARB_extensions_string 
    * WGL_ARB_make_current_read 
    * WGL_ARB_multisample 
    * WGL_ARB_pbuffer 
    * WGL_ARB_pixel_format 
    * WGL_ARB_pixel_format_float 
    * WGL_ARB_render_texture 
    * WGL_ATI_pixel_format_float 
    * WGL_EXT_colorspace 
    * WGL_EXT_create_context_es_profile 
    * WGL_EXT_create_context_es2_profile 
    * WGL_EXT_extensions_string 
    * WGL_EXT_framebuffer_sRGB 
    * WGL_EXT_pixel_format_packed_float 
    * WGL_EXT_swap_control 
    * WGL_EXT_swap_control_tear 
    * WGL_NVX_DX_interop 
    * WGL_NV_DX_interop 
    * WGL_NV_DX_interop2 
    * WGL_NV_copy_image 
    * WGL_NV_delay_before_swap 
    * WGL_NV_float_buffer 
    * WGL_NV_multisample_coverage 
    * WGL_NV_multigpu_context 
    * WGL_NV_render_depth_texture 
    * WGL_NV_render_texture_rectangle 
    * */