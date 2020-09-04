#ifndef __CONSOLE_H__
#define __CONSOLE_H__

class Console;

#include <sstream>
#include <iostream>
#include <stdarg.h>

class Console
{
public:
    static void logError(std::string text);
    static void logError(char const* const _Format, ...);
    static void logMessage(std::string text);
    static void logMessage(char const* const _Format, ...);

    static bool EchoMessages;

private:
    Console();
};

#endif