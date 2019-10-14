#ifndef __MESSAGE_BUS__
#define __MESSAGE_BUS__

#include <queue>
#include <vector>

#include "Message.hpp"

using namespace std;

class MessageBus {
public:
    MessageBus();
    ~MessageBus();

    void processEntireQueue();
    void PopQueue();
    void PostMessage(Message msg);

    void PostMessageByType(MessageType type);

    bool hasMessages;

private:
    queue<Message> mq;
};

#endif
