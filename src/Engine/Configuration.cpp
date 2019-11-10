#include "Configuration.hpp"

//Message::Type Configuration::m_currentMessageEnumCounter = 0;
//std::unordered_map<std::string, Message::Type> Configuration::m_messageTypes;
//std::unordered_map<Message::Type, std::string> Configuration::m_messageNames;

Configuration::Configuration() {}

void Configuration::create() {
    Message::registerMessageType("empty");
    Message::registerMessageType("register");
    Message::registerMessageType("log");
}

void Configuration::destroy() {
}

/*void Configuration::registerMessageType(std::string msgType) {
    m_messageTypes[msgType] = m_currentMessageEnumCounter;
    m_messageNames[m_currentMessageEnumCounter] = msgType;
    m_currentMessageEnumCounter++;
}

Message::Type Configuration::getMessageType(std::string msgType) {
    if (m_messageTypes.find(msgType) == m_messageTypes.end()) {
        return 0;
    } else {
        return m_messageTypes[msgType];
    }
}

std::string Configuration::getNameFromMessageType(Message::Type type) {
    if (m_messageNames.find(type) == m_messageNames.end()) {
        return (std::string("unknownMsgType (") + std::to_string(type) + ")");
    } else {
        return m_messageNames[type];
    }
}

void Configuration::listMessageTypes() {
    for (int n = 0; n < m_currentMessageEnumCounter; n++) {
        Console::logMessage("register: " + m_messageNames[n]);
    }
}*/