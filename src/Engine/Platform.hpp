#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
    #include <Windows.h>

    #define BUFSIZE MAX_PATH
#endif

#include <stdio.h>
#include "DataTypes.hpp"

int getDirectory(char* _curDirectory);
void setDirectory(char* _newDirectory);

#endif //ifndef PLATFORM_H
