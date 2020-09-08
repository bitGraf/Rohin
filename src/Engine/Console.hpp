#ifndef __CONSOLE_H__
#define __CONSOLE_H__

class Console;

#include <sstream>
#include <iostream>
#include <stdarg.h>
#include <fstream>

class Console
{
public:
    static void logError(std::string text);
    static void logError(char const* const _Format, ...);
    static void logMessage(std::string text);
    static void logMessage(char const* const _Format, ...);

    static void OpenLogFile(std::string filename);
    static void CloseLogFile();
    static void EnableFileLogging();
    static void DisableFileLogging();

    static bool fileLogging;
    static std::ofstream logFile;
private:
    Console();
};

#endif