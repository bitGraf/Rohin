#include "MessageBus.hpp"

std::queue<Message> MessageBus::mq;
bool MessageBus::hasMessages = true;
std::vector<CoreSystem*> MessageBus::m_systems;
MessageBus::handleMessageFnc MessageBus::globalFunc;

MessageBus::MessageBus() {
}

void MessageBus::create() {
    printf("MessageBus created\n");
    hasMessages = true;
}
void MessageBus::processEntireQueue() {
    while (hasMessages) {
        auto msg = PopQueue();

        processMessage(msg);
    }
}

Message MessageBus::PopQueue() {
    int num = (int)mq.size();
    //printf("%d messages in queue.\n", num);
    hasMessages = false;

    Message msg;

    if (num > 0) {
        //printf("Popping message from queue\n");
        
        msg = mq.front();
        mq.pop();
        num--;

        if (num == 0)
            hasMessages = false;
        else
            hasMessages = true;
    }
    else {
        printf("No messages to pop\n");
    }

    return msg;
}

void MessageBus::sendMessage(Message msg) {
    mq.push(msg);
    hasMessages = true;
    //printf("Message added to queue\n");
}

void MessageBus::registerSystem(CoreSystem* sys) {
    if (sys != nullptr) {
        m_systems.push_back(sys);
    }
}

void MessageBus::processMessage(Message msg) {
    Console::handleMessage(msg);
    for (auto system : m_systems) {
        system->handleMessage(msg);
    }
    globalFunc(msg);
}

void MessageBus::setGlobalMessageHandleCallback(handleMessageFnc f) {
    globalFunc = f;
}