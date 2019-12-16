#include "Message.hpp"

Message::Type Message::m_currentMessageEnumCounter = 0;
std::unordered_map<std::string, Message::Type> Message::m_messageTypes;
std::unordered_map<Message::Type, std::string> Message::m_messageNames;

Message::Message() : 
    type(0),
    numArgs(0),
    data{0}
{}

Message::Message(Type msg_type, u8 num_args, ...) :
    type(msg_type), numArgs(num_args)
{
    if (num_args > 0) {
        assert(num_args <= MAX_ARGS);

        va_list args;

        va_start(args, num_args);
        for (u8 n = 0; n < num_args; n++) {
            data[n] = va_arg(args, Datatype);
        }
        va_end(args);
    }
}

Message::Message(std::string msg_type_str, u8 num_args, ...) :
    numArgs(num_args)
{
    type = getMessageType(msg_type_str);

    if (num_args > 0) {
        assert(num_args <= MAX_ARGS);

        va_list args;

        va_start(args, num_args);
        for (u8 n = 0; n < num_args; n++) {
            data[n] = (unsigned short)va_arg(args, int);
        }
        va_end(args);
    }
}

bool Message::isType(std::string typeName) {
    return (type == getMessageType(typeName));
}


void Message::registerMessageType(std::string msgType) {
    m_messageTypes[msgType] = m_currentMessageEnumCounter;
    m_messageNames[m_currentMessageEnumCounter] = msgType;
    m_currentMessageEnumCounter++;
}

Message::Type Message::getMessageType(std::string msgType) {
    if (m_messageTypes.find(msgType) == m_messageTypes.end()) {
        return 0;
    }
    else {
        return m_messageTypes[msgType];
    }
}

std::string Message::getNameFromMessageType(Message::Type type) {
    if (m_messageNames.find(type) == m_messageNames.end()) {
        return (std::string("unknownMsgType (") + std::to_string(type) + ")");
    }
    else {
        return m_messageNames[type];
    }
}

void Message::listMessageTypes() {
    for (int n = 0; n < m_currentMessageEnumCounter; n++) {
        Console::logMessage("register: " + m_messageNames[n]);
    }
}
