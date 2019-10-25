#ifndef __MESSAGE_BUS__
#define __MESSAGE_BUS__

#include <queue>
#include <vector>

#include "Message.hpp"

#include "CoreSystem.hpp"
class CoreSystem;

class MessageBus {
public:
    MessageBus();
    ~MessageBus();

    void create();

    //void SetConsole(Console* _console);
    void RegisterSystem(CoreSystem* sys);

    void processEntireQueue();
    Message PopQueue();
    void putMessage(Message msg);

    void PostMessageByType(Message::Type type);
    bool hasMessages;

private:
    std::queue<Message> mq;

    //Console* m_console;
    std::vector<CoreSystem*> systems;

    void processMessage(Message msg);
};

#endif
