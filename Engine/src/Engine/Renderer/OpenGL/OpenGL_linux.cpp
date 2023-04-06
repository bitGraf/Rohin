#include "OpenGL_Platform.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Platform/Platform.h"

#if RH_PLATFORM_LINUX

#include <cstring>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
static bool isExtensionSupported(const char *extList, const char *extension) {
  const char *start;
  const char *where, *terminator;
  
  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return false;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for (start=extList;;) {
    where = strstr(start, extension);

    if (!where)
      break;

    terminator = where + strlen(extension);

    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return true;

    start = terminator;
  }

  return false;
}

static bool ctxErrorOccurred = false;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev ) {
    ctxErrorOccurred = true;
    return 0;
}

bool32 OpenGL_create_context() {
    // Get a matching FB config
    static int visual_attribs[] = {
        GLX_X_RENDERABLE    , True,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        GLX_RED_SIZE        , 8,
        GLX_GREEN_SIZE      , 8,
        GLX_BLUE_SIZE       , 8,
        GLX_ALPHA_SIZE      , 8,
        GLX_DEPTH_SIZE      , 24,
        GLX_STENCIL_SIZE    , 8,
        GLX_DOUBLEBUFFER    , True,
        //GLX_SAMPLE_BUFFERS  , 1,
        //GLX_SAMPLES         , 4,
        None
    };
    
    int glx_major, glx_minor;

    // FBConfigs were added in GLX version 1.3.
    Display* display = (Display*)platform_get_raw_handle();
    if ( !glXQueryVersion( display, &glx_major, &glx_minor ) || 
       ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) ) {
        RH_FATAL("Invalid GLX version");
        return false;
    }
    RH_INFO("Valid GLX version");

    RH_INFO( "Getting matching framebuffer configs" );
    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
    if (!fbc) {
        RH_FATAL( "Failed to retrieve a framebuffer config" );
        return false;
    }
    RH_INFO( "Found %d matching FB configs.", fbcount );

    // Pick the FB config/visual with the most samples per pixel
    RH_INFO( "Getting XVisualInfos" );
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    int i;
    for (i=0; i<fbcount; ++i)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
        if ( vi )
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
            glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
            
            RH_INFO( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
                     " SAMPLES = %d", 
                     i, vi -> visualid, samp_buf, samples );

            if ( best_fbc < 0 || (samp_buf && samples > best_num_samp) )
                best_fbc = i, best_num_samp = samples;
            if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
                worst_fbc = i, worst_num_samp = samples;
        }
        XFree( vi );
    }

    GLXFBConfig bestFbc = fbc[ best_fbc ];

    // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
    XFree( fbc );

    // Get the default screen's GLX extension list
    const char *glxExts = glXQueryExtensionsString( display, DefaultScreen( display ) );
    //RH_DEBUG(glxExts);

    // NOTE: It is not necessary to create or make current to a context before
    // calling glXGetProcAddressARB
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
        glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

    GLXContext ctx = 0;

    // Install an X error handler so the application won't exit if GL 3.0
    // context allocation fails.
    //
    // Note this error handler is global.  All display connections in all threads
    // of a process use the same error handler, so be sure to guard against other
    // threads issuing X commands while this code is running.
    ctxErrorOccurred = false;
    int (*oldHandler)(Display*, XErrorEvent*) =
        XSetErrorHandler(&ctxErrorHandler);

    // Check for the GLX_ARB_create_context extension string and the function.
    // If either is not present, use GLX 1.3 context creation method.
    if ( !isExtensionSupported( glxExts, "GLX_ARB_create_context" ) ||
         !glXCreateContextAttribsARB ) {
        RH_INFO( "glXCreateContextAttribsARB() not found"
                 " ... using old-style GLX context" );
        ctx = glXCreateNewContext( display, bestFbc, GLX_RGBA_TYPE, 0, True );
    }

    // If it does, try to get a GL 3.0 context!
    else {
        int context_attribs[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                None
            };

        RH_INFO( "Creating context" );
        ctx = glXCreateContextAttribsARB( display, bestFbc, 0,
                                                                            True, context_attribs );

        // Sync to ensure any errors generated are processed.
        XSync( display, False );
        if ( !ctxErrorOccurred && ctx ) {
            RH_INFO( "Created GL 3.0 context" );
        } else {
            // Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
            // When a context version below 3.0 is requested, implementations will
            // return the newest context version compatible with OpenGL versions less
            // than version 3.0.
            // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
            context_attribs[1] = 1;
            // GLX_CONTEXT_MINOR_VERSION_ARB = 0
            context_attribs[3] = 0;

            ctxErrorOccurred = false;

            RH_INFO( "Failed to create GL 3.0 context"
                     " ... using old-style GLX context" );
            ctx = glXCreateContextAttribsARB( display, bestFbc, 0, True, context_attribs );
        }
    }

    // Sync to ensure any errors generated are processed.
    XSync( display, False );

    // Restore the original error handler
    XSetErrorHandler( oldHandler );

    if ( ctxErrorOccurred || !ctx )
    {
        RH_INFO( "Failed to create an OpenGL context" );
        return false;
    }

    // Verifying that context is a direct context
    if ( ! glXIsDirect ( display, ctx ) )
    {
        RH_INFO( "Indirect GLX rendering context obtained" );
    }
    else
    {
        RH_INFO( "Direct GLX rendering context obtained" );
    }

    RH_INFO( "Making context current" );
    Window win = platform_get_window_id();
    glXMakeCurrent( display, win, ctx );
    RH_INFO( "Context current" );

    glClearColor( 0, 0.5, 1, 1 );
    RH_INFO( "Clear Color set" );
    glClear( GL_COLOR_BUFFER_BIT );
    RH_INFO( "Color Bit cleared" );
    glXSwapBuffers ( display, win );
    RH_INFO( "Buffers Swapped" );

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