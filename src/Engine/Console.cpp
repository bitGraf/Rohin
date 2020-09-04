#include "Console.hpp"

bool Console::EchoMessages = true;

Console::Console()
{}

void Console::logMessage(std::string text) {
    std::cout << "> " << text << std::endl;
}

void Console::logMessage(char const* const _Format, ...) {
    va_list args;

    printf("> ");

    va_start(args, _Format);
    vprintf(_Format, args);
    va_end(args);

    printf("\n");
}


void Console::logError(std::string text) {
    std::cout << "[ERROR] " << text << std::endl;
}

void Console::logError(char const* const _Format, ...) {
    va_list args;

    printf("[ERROR] ");

    va_start(args, _Format);
    vprintf(_Format, args);
    va_end(args);

    printf("\n");
}