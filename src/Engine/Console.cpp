#include "Console.hpp"

bool Console::fileLogging = false;
std::ofstream Console::logFile;

Console::Console()
{}

void Console::logMessage(std::string text) {
    std::cout << "> " << text << std::endl;

    if (fileLogging && logFile.is_open()) {
        logFile << "> " << text << std::endl;
    }
}

void Console::logMessage(char const* const _Format, ...) {
    va_list args;

    printf("> ");

    va_start(args, _Format);
    //vprintf(_Format, args);
    char buffer[256];
    int numChars = vsprintf(buffer, _Format, args);
    if (numChars < 0)
        printf("error using vsprintf\n");
    printf(buffer);
    va_end(args);

    printf("\n");

    if (fileLogging && logFile.is_open()) {
        logFile << "> " << buffer << std::endl;
    }
}


void Console::logError(std::string text) {
    std::cout << "[ERROR] " << text << std::endl;

    if (fileLogging && logFile.is_open()) {
        logFile << "[ERROR] " << text << std::endl;
    }
}

void Console::logError(char const* const _Format, ...) {
    va_list args;

    printf("[ERROR] ");

    va_start(args, _Format);
    //vprintf(_Format, args);
    char buffer[256];
    int numChars = vsprintf(buffer, _Format, args);
    if (numChars < 0)
        printf("error using vsprintf\n");
    printf(buffer);
    va_end(args);

    printf("\n");

    if (fileLogging && logFile.is_open()) {
        logFile << "> " << buffer << std::endl;
    }
}


void Console::OpenLogFile(std::string filename) {
    if (logFile.is_open()) {
        // close current log file
        logFile.flush();
        logFile.close();
    }

    logFile.open(filename);
    if (logFile.is_open()) {
        logMessage("File opened for logging: %s", filename.c_str());
    }
    else {
        logError("Failed to open file: %s", filename.c_str());
    }
}

void Console::CloseLogFile() {
    if (logFile.is_open()) {
        logFile.flush();
        logFile.close();
    }

    fileLogging = false;
}

void Console::EnableFileLogging() {
    fileLogging = true;
}
void Console::DisableFileLogging() {
    fileLogging = false;
}