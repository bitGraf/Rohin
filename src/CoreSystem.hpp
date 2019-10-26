#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

class CoreSystem;

#include <stdio.h>
#include <stdlib.h>
#include <cstdarg>

#include "Configuration.hpp"
#include "HashTable.hpp"

#include "MessageBus.hpp"

class CoreSystem {
public:
    CoreSystem();
    ~CoreSystem();

    void create(ConfigurationManager* configMgr);
    void setMessageBus(MessageBus* msgBus); // remove this, move into configMgr

    /* Push messages to the queue */
    void putMessage(Message msg);
    void putMessage(Message::Type _type, std::string data);
    void logMessage(const char* text);
    void logMessage(const char* text, int count, ...); // ONLY ALLOWS INTS
    
    /* Virtuals */
    virtual void update(double dt) = 0;
    virtual void handleMessage(Message msg) = 0;
    virtual void destroy() = 0;
    virtual void sys_create(ConfigurationManager* configMgr) = 0;

protected:
    ConfigurationManager*   m_configMgr;
    MessageBus*             m_msgBus;
};

#endif