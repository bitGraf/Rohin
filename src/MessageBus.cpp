#include "MessageBus.hpp"

MessageBus::MessageBus() {
}

void MessageBus::create() {
    printf("MessageBus created\n");
    hasMessages = true;
}

MessageBus::~MessageBus() {

}

void MessageBus::processEntireQueue() {
    while (hasMessages) {
        PopQueue();
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

void MessageBus::_PostMessage(Message msg) {
    mq.push(msg);
    hasMessages = true;
    //printf("Message added to queue\n");
}

void MessageBus::PostMessageByType(MessageType type) {
    Message msg(type, "blank");
    _PostMessage(msg);
}