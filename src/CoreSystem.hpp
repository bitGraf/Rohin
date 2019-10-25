#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__


#include <stdio.h>
#include <stdlib.h>
#include <cstdarg>

#include "Configuration.hpp"
#include "HashTable.hpp"

#include "MessageBus.hpp"
class MessageBus;

class CoreSystem {
public:
    CoreSystem();
    ~CoreSystem();

    void create(ConfigurationManager* configMgr);
    void setMessageBus(MessageBus* msgBus); // remove this, move into configMgr
    void putMessage(Message msg);

    void logMessage(const char* text);
    void logMessage(const char* text, int count, ...); // ONLY ALLOWS INTS
    
    /* Virtuals */
    virtual void update(double dt) = 0;
    virtual void handleMessage(Message msg) = 0;
    virtual void destroy() = 0;
    virtual void sys_create(ConfigurationManager* configMgr) = 0;

private:
    ConfigurationManager*   m_configMgr;
    MessageBus*             m_msgBus;
};

#endif