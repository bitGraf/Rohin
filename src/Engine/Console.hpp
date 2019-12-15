#ifndef __CONSOLE_H__
#define __CONSOLE_H__

class Console;

#include "Message/Message.hpp"

#include <sstream>
#include <iostream>

class Console
{
public:
    static void handleMessage(Message msg);
    static void logMessage(std::string text);
    static void logMessage(char const* const _Format, ...);

    static bool EchoMessages;

private:
    Console();
};

#endif