#include "MessageBus.hpp"

MessageBus::MessageBus() {
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

void MessageBus::PopQueue() {
    int num = (int)mq.size();
    //printf("%d messages in queue.\n", num);

    if (num > 0) {
        //printf("Popping message from queue\n");
        
        Message msg = mq.front();
        mq.pop();
        num--;

        printf("Message type: ");
        switch (msg.type) {
        case MessageType::empty: {
            printf("Empty\n");
            } break;
        case MessageType::systemCreation: {
            printf("System Created [%s]\n", msg.text.c_str());
            } break;
        case MessageType::type2: {
            printf("Type 2\n");
            } break;
        }

        if (num == 0)
            hasMessages = false;
        else
            hasMessages = true;
    }
    else {
        printf("No messages to process\n");
    }
}

void MessageBus::PostMessage(Message msg) {
    mq.push(msg);
    //printf("Message added to queue\n");
}

void MessageBus::PostMessageByType(MessageType type) {
    Message msg(type);
    PostMessage(msg);
}