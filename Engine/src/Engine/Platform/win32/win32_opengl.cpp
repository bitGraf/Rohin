#include "win32_opengl.h"

const char* 
Win32GetWGLExtensionString(HDC WindowDC) {
    _wglGetExtensionsStringARB_PROC wglGetExtensionsStringARB = (_wglGetExtensionsStringARB_PROC)wglGetProcAddress("wglGetExtensionsStringARB");
    if (wglGetExtensionsStringARB) {
        const char* extensions = wglGetExtensionsStringARB(WindowDC);
        if (extensions) {
            OutputDebugStringA("WGL Extensions:\n  ");
            OutputDebugStringA(extensions);
            OutputDebugStringA("\n");
            return extensions;
        }
    }

    return 0;
}

static void
Win32InitOpenGL(HWND Window) {
    HDC WindowDC = GetDC(Window);

    /*
        * PixelFormat negotion process:
        * 
        * Create a PIXELFORMATDESCRIPTOR that is partially filled out, 
        * with the properties that we ~want~ as the pixel format.
        * 
        * run :
        * int SuggestedPixelFormatIndex = ChoosePixelFormat(WindowDC, &DesiredPixelFormat);
        * to get an index 
        * to an ~actual~ pixel format that matches (hopefully) closely 
        * to the one we asked for.
        * 
        * We have an index to a pixelformat, use DescribePixelFormat(...,SuggestedPixelFormatIndex,...)
        * to get the actual pixelformat from win32.
        * 
        * Finally... SetPixelFormat() with the actual pixelformat from win32
        * 
        * We then have to do this again with the escalated context...
        */

    PIXELFORMATDESCRIPTOR DesiredPixelFormat = {
                                               sizeof(PIXELFORMATDESCRIPTOR),
                                               1,
                                               PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
                                               PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
                                               32,                   // Colordepth of the framebuffer.
                                               0, 0, 0, 0, 0, 0,     // r/g/b bits/shift
                                               0,                    // alpha bits
                                               0,                    // alpha shift
                                               0,                    // accum bits
                                               0, 0, 0, 0,           // accum r/g/b/a bits
                                               24,                   // Number of bits for the depthbuffer
                                               8,                    // Number of bits for the stencilbuffer
                                               0,                    // Number of Aux buffers in the framebuffer.
                                               PFD_MAIN_PLANE,       // Layer type
                                               0,                    // bReserved
                                               0, 0, 0               // Masks
    };

    int SuggestedPixelFormatIndex = ChoosePixelFormat(WindowDC, &DesiredPixelFormat);
    PIXELFORMATDESCRIPTOR SuggestedPixelFormat;
    DescribePixelFormat(WindowDC, SuggestedPixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &SuggestedPixelFormat);
    SetPixelFormat(WindowDC, SuggestedPixelFormatIndex, &SuggestedPixelFormat);

    // create a dummy context to get access to wgl extensions... to create the real context
    HGLRC DummyOpenGLRC = wglCreateContext(WindowDC);
    if (wglMakeCurrent(WindowDC, DummyOpenGLRC)) {
        const char* WGLExtensions = Win32GetWGLExtensionString(WindowDC);
        Assert(WGLExtensions);

        // Get extension functions from wgl
        _wglCreateContextAttribsARB_PROC   wglCreateContextAttribsARB = (_wglCreateContextAttribsARB_PROC)wglGetProcAddress("wglCreateContextAttribsARB");
        _wglGetPixelFormatAttribivARB_PROC wglGetPixelFormatAttribivARB = (_wglGetPixelFormatAttribivARB_PROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
        _wglGetPixelFormatAttribfvARB_PROC wglGetPixelFormatAttribfvARB = (_wglGetPixelFormatAttribfvARB_PROC)wglGetProcAddress("wglGetPixelFormatAttribfvARB");
        _wglChoosePixelFormatARB_PROC      wglChoosePixelFormatARB = (_wglChoosePixelFormatARB_PROC)wglGetProcAddress("wglChoosePixelFormatARB");

        const int ARBPixelFormatAttribList[] = {
                                 WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                                 WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                                 WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                                 WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                                 WGL_COLOR_BITS_ARB, 32,
                                 WGL_DEPTH_BITS_ARB, 24,
                                 WGL_STENCIL_BITS_ARB, 8,
                                 0, // End
        };

        int ARBSuggestedPixelFormatIndex;
        UINT NumFormats;
        
        bool32 ARBResult = wglChoosePixelFormatARB(WindowDC, ARBPixelFormatAttribList, NULL, 1, &ARBSuggestedPixelFormatIndex, &NumFormats);
        Assert(ARBResult && (ARBSuggestedPixelFormatIndex == SuggestedPixelFormatIndex));

        // now create a higher-version context
        const int ARBContexAttrivbs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 0,
            WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_DEBUG_BIT_ARB|WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0 // End
        };

#if 1
        HGLRC ActualOpenGLRC = wglCreateContextAttribsARB(WindowDC, 0, ARBContexAttrivbs);
        if (ActualOpenGLRC) {
            // able to create an OpenGL 4.0 context!
            wglMakeCurrent(WindowDC, ActualOpenGLRC);

            // delete the dummy context too
            wglDeleteContext(DummyOpenGLRC);
        } else {
            // Stuck with an OpenGL 1.0 context ;~;
            ActualOpenGLRC = DummyOpenGLRC;
            Assert(!"Not supported!");
        }
#else
        HGLRC ActualOpenGLRC = DummyOpenGLRC;
#endif

        OutputDebugStringA("OpenGL Context created!\n");


        const char* glVendorString     = (const char*) glGetString(GL_VENDOR);
        const char* glRendererString   = (const char*) glGetString(GL_RENDERER);
        const char* glVersionString    = (const char*) glGetString(GL_VERSION);

        OutputDebugStringA(glVendorString); OutputDebugStringA("\n");
        OutputDebugStringA(glRendererString); OutputDebugStringA("\n");
        OutputDebugStringA(glVersionString); OutputDebugStringA("\n");

#if 0
        _glGetStringi_PROC glGetStringi = (_glGetStringi_PROC)wglGetProcAddress("glGetStringi");
        GLint num_extensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
        OutputDebugStringA("OpenGL Extensions:");
        for (GLint n = 0; n < num_extensions; n++) {
            const char* glExtensionsString = (const char*) glGetStringi(GL_EXTENSIONS, n);
            OutputDebugStringA("  ");
            OutputDebugStringA(glExtensionsString);
            OutputDebugStringA("\n");
        }
#endif

        // Set VSync
        _wglSwapIntervalEXT_PROC wglSwapIntervalEXT = (_wglSwapIntervalEXT_PROC)wglGetProcAddress("wglSwapIntervalEXT");
        _wglGetSwapIntervalEXT_PROC wglGetSwapIntervalEXT = (_wglGetSwapIntervalEXT_PROC)wglGetProcAddress("wglGetSwapIntervalEXT");

        wglSwapIntervalEXT(2); // since my monitor is 120Hz, interval of 2 to lock to 60fps

    } else {
        Assert(!"InvalidCodePath");
    }
    ReleaseDC(Window, WindowDC);
}