#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class ConfigurationManager;

#include <unordered_map>

#include "MessageBus.hpp"

class Configuration {
public:
    static void create();
    static void destroy();

    static void registerMessageType(std::string msgType);
    static Message::Type getMessageType(std::string msgType);
    static std::string getNameFromMessageType(Message::Type type);

    static Message encodeData(std::string msgType, int d1, int d2);
    static void decodeData(Message msg, int& out1, int& out2);

    static void listMessageTypes();
       
private:
    Configuration();

    static std::unordered_map<std::string, Message::Type> m_messageTypes;
    static std::unordered_map<Message::Type, std::string> m_messageNames;
    static Message::Type m_currentMessageEnumCounter;
};

#endif
