#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <stdio.h>
#include <stdlib.h>

#include "types.hpp"

class System {
public:
    System(const char* _name);
    ~System();

    void handleMessage();
    void update();

private:
    //MessageBus *msgBus;
    // ?  Usage: msgBus->postMessage(msg);
    uint counter = 0;

    char* name;
    uint nameLength;
};

#endif