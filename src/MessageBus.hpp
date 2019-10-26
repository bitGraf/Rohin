#ifndef __MESSAGE_BUS__
#define __MESSAGE_BUS__

class MessageBus;

#include <queue>
#include <vector>

#include "Message.hpp"
#include "CoreSystem.hpp"

class MessageBus {
public:
    MessageBus();
    ~MessageBus();

    void create(ConfigurationManager* configMgr);

    void processEntireQueue();
    Message PopQueue();
    void putMessage(Message msg);

    void PostMessageByType(Message::Type type);
    bool hasMessages;

private:
    ConfigurationManager* m_configManager;
    std::queue<Message> mq;

    void processMessage(Message msg);
};

#endif
