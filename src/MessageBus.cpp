#include "MessageBus.hpp"

MessageBus::MessageBus() {
}

void MessageBus::create(ConfigurationManager* configMgr) {
    printf("MessageBus created\n");
    hasMessages = true;
    m_configManager = configMgr;

    for (auto sys : configMgr->m_coreSystems) {
        sys->setMessageBus(this);
    }
}

MessageBus::~MessageBus() {

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

void MessageBus::putMessage(Message msg) {
    mq.push(msg);
    hasMessages = true;
    //printf("Message added to queue\n");
}

void MessageBus::PostMessageByType(Message::Type type) {
    Message msg(type, "blank");
    putMessage(msg);
}

void MessageBus::processMessage(Message msg) {
    //m_console->handleMessage(msg);
    for (auto system : m_configManager->m_coreSystems) {
        system->handleMessage(msg);
    }
}