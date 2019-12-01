#include "Console.hpp"

bool Console::EchoMessages = true;

Console::Console()
{}

void Console::handleMessage(Message msg) {
    if (EchoMessages) {
        std::ostringstream stringStream;

        stringStream <<
            Message::getNameFromMessageType(msg.type);
        for (u8 n = 0; n < msg.numArgs; n++) {
            stringStream << ", " << std::to_string(msg.data[n]);
        }
        std::string copyOfStr = stringStream.str();
        
        if (!msg.isType("InputKey")) // Don't need to flood the console
            logMessage(copyOfStr);
    }
}

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