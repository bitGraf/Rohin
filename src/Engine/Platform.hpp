#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
    #include <direct.h>
    #define cwd _getcwd
    #define cd _chdir
#else
    #include "unistd.h"
    #define cwd getcwd
    #define cd chdir
#endif

#include <stdio.h>
#include "DataTypes.hpp"

#include "Console.hpp"

#endif //ifndef PLATFORM_H
