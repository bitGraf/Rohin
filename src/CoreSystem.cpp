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
    //m_msgBus = configMgr->getMessageBus();

    /* Pass on to specific implementation */
    sys_create(configMgr);
}

void CoreSystem::putMessage(Message msg) {
    if (m_msgBus) {
        m_msgBus->putMessage(msg);
    }
}

void CoreSystem::logMessage(const char* text) {
    Message msg;
    msg.type = Message::Type::log;
    msg.text = std::string(text);
    putMessage(msg);
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