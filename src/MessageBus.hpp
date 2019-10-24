#ifndef __MESSAGE_BUS__
#define __MESSAGE_BUS__

#include <queue>
#include <vector>

#include "Message.hpp"

using namespace std;

#include "Console.hpp"
class Console;

class MessageBus {
public:
    MessageBus();
    ~MessageBus();

    void create();

    void SetConsole(Console* _console);

    void processEntireQueue();
    Message PopQueue();
    void _PostMessage(Message msg);

    void PostMessageByType(MessageType type);
    bool hasMessages;

private:
    queue<Message> mq;

    Console* m_console;

    void processMessage(Message msg);
};

#endif
