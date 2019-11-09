#include "Configuration.hpp"

Message::Type Configuration::m_currentMessageEnumCounter = 0;
std::unordered_map<std::string, Message::Type> Configuration::m_messageTypes;
std::unordered_map<Message::Type, std::string> Configuration::m_messageNames;

Configuration::Configuration() {}

void Configuration::create() {    
    registerMessageType("empty");
    registerMessageType("register");
    registerMessageType("log");
}

void Configuration::destroy() {
}

void Configuration::registerMessageType(std::string msgType) {
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

Message Configuration::encodeData(std::string msgType, int d1, int d2) {
    Message msg;

    msg.type = getMessageType(msgType);

    // Pack two ints into data block
    u16 val1 = (d1);
    u16 val2 = (d2);

    memcpy(msg.data, &val1, 2);
    memcpy(msg.data + 2, &val2, 2);

    return msg;
}

void Configuration::decodeData(Message msg, int& out1, int& out2) {
    // decode message datablock
    u16 val1, val2;
    memcpy(&val1, msg.data, 2);
    memcpy(&val2, msg.data + 2, 2);

    out1 = static_cast<int>(val1);
    out2 = static_cast<int>(val2);
}

void Configuration::listMessageTypes() {
    for (int n = 0; n < m_currentMessageEnumCounter; n++) {
        Message msg;
        msg.type = getMessageType("register");
        msg.text = m_messageNames[n];
        MessageBus::sendMessage(msg);
    }
}