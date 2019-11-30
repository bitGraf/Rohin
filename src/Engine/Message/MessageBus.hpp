#ifndef __MESSAGE_BUS__
#define __MESSAGE_BUS__

class MessageBus;

#include <queue>
#include <vector>

#include "Message.hpp"
#include "CoreSystem.hpp"
#include "Console.hpp"
#include "Input.hpp"

typedef void(*handleMessageFnc)(Message);

class MessageBus {
public:
    static void create();

    static void processEntireQueue();
    static Message PopQueue();
    static void sendMessage(Message msg);

    static void registerSystem(CoreSystem* sys);

    static void setGlobalMessageHandleCallback(handleMessageFnc f);

private:
    static std::queue<Message> mq;
    static std::vector<CoreSystem*> m_systems;
    static bool hasMessages;

    static void processMessage(Message msg);
    static handleMessageFnc globalFunc;

    MessageBus();
};

#endif
