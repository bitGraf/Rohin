#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <stdio.h>
#include <stdlib.h>

#include "types.hpp"
#include "MessageBus.hpp"

class System {
public:
    System(const char* _name, MessageBus* _msgBus);
    ~System();

    void handleMessage(Message* msg);
    void update();

    char* name;

    uint counter = 0;
private:
    MessageBus *msgBus;
    // ?  Usage: msgBus->postMessage(msg);
    uint nameLength;
};

#endif