#ifndef PLATFORM_H
#define PLATFORM_H

/* Define platform macros based on current build platform */
#ifdef _WIN32
    /* Windows x64/x86 */
    #ifdef _WIN64
        /* Windows x64  */
        #define RH_PLATFORM_WINDOWS
    #else
        /* Windows x86 */
        //#error "x86 Builds are not supported!"
        #define RH_PLATFORM_WINDOWS
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    /* TARGET_OS_MAC exists on all the platforms
    * so we must check all of them (in this order)
    * to ensure that we're running on MAC
    * and not some other Apple platform */
    #if TARGET_IPHONE_SIMULATOR == 1
        #error "IOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1
        #define RH_PLATFORM_IOS
        #error "IOS is not supported!"
    #elif TARGET_OS_MAC == 1
        #define RH_PLATFORM_MACOS
        #error "MacOS is not supported!"
    #else
        #error "Unknown Apple platform!"
    #endif
/* We also have to check __ANDROID__ before __linux__
* since android is based on the linux kernel
* it has __linux__ defined */
#elif defined(__ANDROID__)
    #define RH_PLATFORM_ANDROID
    #error "Android is not supported!"
#elif defined(__linux__)
    #define RH_PLATFORM_LINUX
    #error "Linux is not supported!"
#else
    /* Unknown compiler/platform */
    #error "Unknown platform!"
#endif // End of platform detection


/* FileSystem functions */
#ifdef _WIN32
    #include <direct.h>
    #define cwd _getcwd
    #define cd _chdir
#else
    #include "unistd.h"
    #define cwd getcwd
    #define cd chdir
#endif

#endif //ifndef PLATFORM_H
