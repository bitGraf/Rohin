#include "CoreSystem.hpp"

CoreSystem::CoreSystem() {
}

CoreSystem::~CoreSystem() {
}

void CoreSystem::sendMessage(Message msg) {
    MessageBus::sendMessage(msg);
}

void CoreSystem::sendMessage(Message::Type _type, std::string data) {
        Message msg;
        msg.type = _type;
        msg.text = data;
        MessageBus::sendMessage(msg);
}

void CoreSystem::sendMessage(std::string msgType) {
    sendMessage(Configuration::getMessageType(msgType), "");
}

void CoreSystem::logMessage(const char* text) {
    sendMessage(99, std::string(text));
}

void CoreSystem::logMessage(const char* text, int count, ...) {
    va_list args;
    va_start(args, count);

    std::string m(text);

    for (int n = 0; n < count - 1; n++) {
        int num = va_arg(args, int);
        m.append(std::to_string(num));
        m.append(", ");
    }
    int num = va_arg(args, int);
    m.append(std::to_string(num));

    logMessage(m.c_str());
}