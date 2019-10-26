#include "CoreSystem.hpp"

CoreSystem::CoreSystem() {
    m_configMgr = nullptr;
    m_msgBus    = nullptr;
}

CoreSystem::~CoreSystem() {

}

void CoreSystem::setMessageBus(MessageBus* msgBus) {
    m_msgBus = msgBus;
}

void CoreSystem::create(ConfigurationManager* configMgr) {
    /* Do generic system configuration */
    m_configMgr = configMgr;

    /* Pass on to specific implementation */
    sys_create(configMgr);
}

void CoreSystem::putMessage(Message msg) {
    if (m_msgBus) {
        m_msgBus->putMessage(msg);
    }
}

void CoreSystem::putMessage(Message::Type _type, std::string data) {
    if (m_msgBus) {
        Message msg;
        msg.type = _type;
        msg.text = data;
        m_msgBus->putMessage(msg);
    }
}

void CoreSystem::logMessage(const char* text) {
    putMessage(Message::Type::log, std::string(text));
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